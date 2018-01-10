// Autor: Michał Artur Krawczak

#include "VirtualFS.h"

using namespace std;
using namespace boost;
using namespace filesystem;

VirtualFS::VirtualFS(string name):
  _name(name)
{
  open();
}

VirtualFS::~VirtualFS()
{
  close();
}

void VirtualFS::create_fs_file(unsigned size)
{
  if(size <= SYSTEM_BLOCKS)
  {
    throw DiscTooSmall(); // dysk zbyt maly
  }
  _size = size;
  buffer buf;
  for(unsigned i=0; i<BLOCK_SIZE; ++i)
  {
    buf[i] = '\0';
  }
  total_blocks_used = SYSTEM_BLOCKS;
  fstream ostr;
  ostr.open(_name.c_str(), ios::out|ios::binary);
  for(unsigned i=0; i<size; ++i)
  {
    ostr.write(buf, BLOCK_SIZE);
  }
  ostr.close();
}

void VirtualFS::open_fs_file()
{
  _size = file_size(_name) / BLOCK_SIZE;
  buffer buf[SYSTEM_BLOCKS];
  ifstream istr;
  istr.open(_name.c_str(), ios::in|ios::binary);
  for(unsigned j=0; j < SYSTEM_BLOCKS; j++)
  {
    istr.read(buf[j], BLOCK_SIZE);
  }
  total_blocks_used = SYSTEM_BLOCKS;
  for(unsigned j=0; j < SYSTEM_BLOCKS; j++)
  {
    for(unsigned i=0; i < BLOCK_SIZE; i+=sizeof(inode))
    {
      inode new_inode = *reinterpret_cast<inode *>(buf[j] + i);
      if(new_inode.used)
      {
        inodes.push_back(new_inode);
        total_blocks_used += new_inode.blocks;
      }
    }
  }
  istr.close();
}

void VirtualFS::rename_file(string old_name, string new_name)
{
  if(find_inode_by_name(new_name) != NULL)
  {
    throw FileNameNotUnique(); // plik o takiej nazwie juz istnieje
  }
  inode *node;
  if((node = find_inode_by_name(old_name)) == NULL)
  {
    throw FileDoesntExist(); // nie ma takiego pliku
  }
  strcpy(node->name, new_name.c_str());
}

void VirtualFS::defragment()
{
  // szukamy dziury w calym
  unsigned index = 0,
           new_pos = SYSTEM_BLOCKS;
  // jesli inodes[0].begin != SYSTEM_BLOCKS, to przesuwamy pierwszy plik
  if(inodes[0].begin == SYSTEM_BLOCKS)
  {
    // jesli nie, to szukamy dziury miedzy dwoma plikami
    for(index = 1; index < inodes.size(); ++index)
    {
      if(inodes[index-1].end() < inodes[index].begin)
      {
        // jest dziura
        new_pos = inodes[index-1].end();
        break;
      }
    }
  }
  // index = numer pliku do przesuniecia
  // new_pos = nowy numer bloku startowego dla pliku
  unsigned blocks = inodes[index].blocks;
  unsigned old_pos = inodes[index].begin;
  inodes[index].begin = new_pos;
  buffer buffers[blocks];
  ifstream istr;
  istr.open(_name.c_str());
  istr.seekg(old_pos * BLOCK_SIZE);
  for(unsigned i = 0; i < blocks; ++i)
  {
    istr.read(buffers[i], BLOCK_SIZE);
  }
  fstream ostr;
  ostr.open(_name.c_str(), ios::in|ios::out|ios::binary);
  ostr.seekp(new_pos * BLOCK_SIZE);
  for(unsigned i = 0; i < blocks; ++i)
  {
    ostr.write(buffers[i], BLOCK_SIZE);
  }
}

void VirtualFS::remove_file(std::string file_name)
{
  inode *node;
  if((node = find_inode_by_name(file_name)) == NULL)
  {
    throw FileDoesntExist(); // nie ma takiego pliku
  }
  // usuwamy inode z tablicy
  swap(*node, inodes.back());
  inodes.pop_back();
}

void VirtualFS::map()
{
  char tab[_size];
  for(unsigned i = 0; i < SYSTEM_BLOCKS; ++i)
  {
    tab[i] = '+';
  }
  for(unsigned i = SYSTEM_BLOCKS; i < _size; ++i)
  {
    tab[i] = '.';
  }
  for(unsigned j = 0; j < inodes.size(); ++j)
  {
    for(unsigned i = inodes[j].begin; i < inodes[j].end(); ++i)
    {
      tab[i] = inodes[j].name[0];
    }
  }
  cout<< "Legenda: " << endl
      << "  '.' = blok wolny" << endl 
      << "  '+' = blok systemowy (zarezerwowany na deskryptory plikow)" << endl
      << "  'w' = blok zajety przez plik (w - pierwsza litera nazwy)" << endl
      << "1 blok = "<< BLOCK_SIZE << " bajty/ow" << endl << endl;  
      
  for(unsigned i = 0; i < _size; ++i)
  {
    cout << tab[i];
    if(!((i+1)&31))
    {
      cout << endl;
    }
  }
  cout << endl;
}

void VirtualFS::list_files()
{
  cout << setw(10) << "start" 
       << setw(10) << "koniec" 
       << setw(10) << "rozmiar" 
       << setw(10) << "rozmiar" 
       << setw(30) << "nazwa" << endl;
  cout << setw(10) << "[blok]" 
       << setw(10) << "[blok]"   
       << setw(10) << "[bloki]"   
       << setw(10) << "[bajty]" << endl;
  for(unsigned i = 0; i < inodes.size(); ++i)
  {
    cout << setw(10) << inodes[i].begin
         << setw(10) << inodes[i].end()-1
         << setw(10) << inodes[i].blocks
         << setw(10) << inodes[i].size
         << setw(30) << inodes[i].name << endl;
  }
}

void VirtualFS::open()
{
  if(get_title()[0x028]!=0x063)
  {
    _name = "/";
  }
}

void VirtualFS::download_file(string dest_file)
{
  inode *node;
  if((node = find_inode_by_name(dest_file)) == NULL)
  {
    throw FileDoesntExist(); // nie ma takiego pliku
  }
  int counter = (int)node->size;
  buffer buf;
  ofstream dest;
  dest.open(dest_file.c_str());
  ifstream src;
  src.open(_name.c_str());
  src.seekg(node->begin * BLOCK_SIZE);
  unsigned curr_block_size = BLOCK_SIZE;
  while(counter > 0)
  {
    if(counter < (int)BLOCK_SIZE)
    {
      curr_block_size = counter;
    }
    src.read(buf, curr_block_size);
    dest.write(buf, curr_block_size);
    counter -= curr_block_size;
  }
  dest.close();
  src.close();
}
unsigned VirtualFS::alloc(unsigned blocks)
{
  while(1)
  {
    unsigned nodes = inodes.size();
    if(nodes == 0 || inodes[0].begin - SYSTEM_BLOCKS >= blocks)
    // czy jest miejsce na poczatku?
    {
      return SYSTEM_BLOCKS;
    }
    for(unsigned i = 1; i < inodes.size(); ++i)
    {
      unsigned hole = inodes[i].begin - inodes[i-1].end();
      if(hole >= blocks)
      {
        return inodes[i-1].end();
      }
    }
    if(nodes != 0 && _size - inodes[nodes-1].end() >= blocks)
      // no to moze chociaz na koncu?
    {
      return inodes[nodes-1].end();
    }
    // jesli nie udalo sie znalezc miejsca, to troche posprzatajmy
    defragment();
  }
}

int VirtualFS::cmp_inodes(inode a, inode b)
{
  return a.begin < b.begin;
}

void VirtualFS::close()
{
  sort(inodes.begin(), inodes.end(), cmp_inodes);
  buffer buffers[SYSTEM_BLOCKS];
  unsigned index = 0;
  for(unsigned j=0; j < SYSTEM_BLOCKS; j++)
  {
    for(unsigned i=0; i < BLOCK_SIZE; i+=sizeof(inode), index++)
    {
      // ponizsza linijka ma prawo nie dzialac
      inode *ptri = reinterpret_cast<inode*>(buffers[j] + i);
      if(index < inodes.size())
      {
        *ptri = inodes[index];
      }
      else
      {
        // zerujemy pozostale deskryptory, na wszelki wypadek...
        for(unsigned k=0; k < sizeof(inode); ++k)
        {
          *(buffers[j] + i + k) = 0;
        }
      }
    }
  }
  fstream ostr;
  ostr.open(_name.c_str(), ios::in|ios::out|ios::binary);
  ostr.seekp(ios_base::beg);
  for(unsigned j=0; j < SYSTEM_BLOCKS; j++)
  {
    ostr.write(buffers[j], BLOCK_SIZE);
  }
}

VirtualFS::inode *VirtualFS::find_inode_by_name(string source)
{
  for(unsigned i=0; i<inodes.size(); ++i)
  {
    if(inodes[i].name == source)
    {
      return &inodes[i];
    }
  }
  return NULL;
}

void VirtualFS::upload_file(string source)
{ 
  //int kropka = source.find_last_of('.');
  //string dest;
  //if(kropka != -1)
  //{
  //  dest = 'm' + source.substr(0, kropka) + 'k' + source.substr(kropka); 
  //}
  //else
  //{
  //  dest = 'm' + source + 'k';
  //}
  
  string dest = source; // plik na naszym dysku wirtualnym bedize sie nazywac tak samo jak oryginal
    
  if(find_inode_by_name(dest) != NULL)
  {
    throw FileNameNotUnique(); // plik o takiej nazwie juz istnieje
  }
  unsigned source_size = file_size(source);
  unsigned source_blocks = (source_size - 1) / BLOCK_SIZE + 1; // zaokraglenie w gore
  unsigned total_blocks_free = _size - total_blocks_used;
  if(source_blocks > total_blocks_free)
  {
    throw NotEnoughSpace(); // brak miejsca 
  }
  unsigned position = alloc(source_blocks);
  buffer buffers[source_blocks];
  ifstream istr;
  istr.open(source.c_str());
  for(unsigned i=0; i < source_blocks; ++i)
  {
    istr.read(buffers[i], BLOCK_SIZE);
  }
  fstream ostr;
  ostr.open(_name.c_str(), ios::in|ios::out|ios::binary);
  ostr.seekp(position * BLOCK_SIZE);
  for(unsigned i=0; i < source_blocks; ++i)
  {
    ostr.write(buffers[i], BLOCK_SIZE);
  }
  inode new_inode;
  new_inode.used = 1;
  new_inode.begin = position;
  new_inode.size = source_size;
  new_inode.blocks = source_blocks;
  strcpy(new_inode.name, dest.c_str());
  inodes.push_back(new_inode);
}