// Autor: Michał Artur Krawczak

#include "VirtualFS.h"

using namespace std;
using namespace boost; 
using namespace program_options; 
using namespace filesystem;

int main(int argc, char *argv[]) try
{
  options_description desc ("Dozwolone argumenty",1024,512); 
  desc.add_options() 
    ("file,f", value<string>(), "nazwa pliku z wirtualnym dyskiem (wymagana dla wszystkich opcji oprocz -h)")
    ("create,c", value<unsigned>()->required(), "tworzy nowy (pusty) wirtualny dysk o zadanym rozmiarze (w kB)") 
    ("upload,u", value<string>()->required(), "kopiuje plik o nazwie arg na wirtualny dysk") 
    ("download,d", value<string>()->required(), "pobiera plik o nazwie arg z wirtualnego dysku")
    ("remove,r", value<string>()->required(), "usuwa plik o nazwie arg z wirtualnego dysku")
    ("move,m", value<vector<string> >()->required()->multitoken(), "zmienia nazwe pliku znajdujacego sie na wirtualnym dysku\n"
      "Skladnia: \n"
      "  -m stara_nazwa nowa_nazwa"
    )
    ("list,l", "wyswietla liste plikow")
    ("map,M", "wyswietla mape wolnych i zajetych blokow")
    ("help,h", "wyswietla pomoc (wlasnie ja czytasz)")
  ;
  
  variables_map vm; 
  store(command_line_parser(argc, argv).options(desc).run(), vm); 
  
  if(vm.count("help") || !vm.count("file")) 
  { 
    cout
    << VirtualFS::get_title()
    << "Uzycie:\n\033[1m"
    << "  "<< argv[0] <<" -f plik operacja \n\033[m"
    << desc 
    << endl; 
    return 0; 
  }
  string file = vm["file"].as<string>();
  VirtualFS FS(file);
  if(vm.count("create"))
  {
    unsigned size = vm["create"].as<unsigned>();
    FS.create_fs_file(size);
  }
  else
  {
    FS.open_fs_file();
    if(vm.count("upload"))
    {
      string source_file = vm["upload"].as<string>();
      FS.upload_file(source_file);
    }
    else
    if(vm.count("download"))
    {
      string dest_file = vm["download"].as<string>();
      FS.download_file(dest_file);
    }
    else
    if(vm.count("move"))
    {
      string old_name = vm["move"].as<vector<string> >()[0];
      string new_name = vm["move"].as<vector<string> >()[1];
      FS.rename_file(old_name, new_name);
    }
    else
    if(vm.count("remove"))
    {
      string file_name = vm["remove"].as<string>();
      FS.remove_file(file_name);
    }
    else
    if(vm.count("list"))
    {
      FS.list_files();
    }
    else
    if(vm.count("map"))
    {
      FS.map();
    }
  }
  return 0;
}
catch(std::exception &e)
{
  cout << e.what() << endl;
  cout << "Aby uzyskac pomoc, wpisz:\n  " << argv[0] <<" -h" << endl;
  return -1;
}