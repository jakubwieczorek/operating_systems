// Autor: Michał Artur Krawczak

#if not defined __VIRTUALFS_H__
#define __VIRTUALFS_H__

#include <fstream>
#include <string>
#include <cstring>
#include <iostream>
#include <iomanip>
#include "boost/filesystem.hpp"
#include "boost/program_options.hpp"

class VirtualFS
{
  struct inode;
  
  static const unsigned BLOCK_SIZE = 1024; // 1kB
  static const unsigned SYSTEM_BLOCKS = 8;
  
  struct inode
  {
    bool used;
    // 3 bajty zarezerwowane dla wyrownania
    unsigned begin;
    unsigned end() { return begin + blocks; }
    unsigned size;
    unsigned blocks;
    char name[48];
  }; // sizeof(inode) = 64
  
  class NotEnoughSpace: public std::exception
  {
    const char* what() const throw() 
    { return "Brak miejsca na wirtualnym dysku."; }
  };
  
  class FileDoesntExist: public std::exception
  {
    const char* what() const throw() 
    { return "Nie istnieje plik o podanej nazwie.\nAby przejrzec liste plikow, uzyj polecenia -l."; }
  };
  
  class FileNameNotUnique: public std::exception
  {
    const char* what() const throw() 
    { return "Plik o takiej samej nazwie juz istnieje na wirtualnym dysku.\nAby zmienic nazwe pliku, uzyj polecenia -m."; }
  };
  
  class DiscTooSmall: public std::exception
  {
    const char* what() const throw() 
    { return "Podany rozmiar wirtualnego dysku jest za maly."; }
  };
  
  std::vector<inode> inodes;
  typedef char buffer [BLOCK_SIZE];
  std::string _name;
  unsigned _size;
  unsigned total_blocks_used;
  
  unsigned alloc(unsigned);
  void defragment();
  void close();
  inode *find_inode_by_name(std::string);
  static int cmp_inodes(inode, inode);
public:
  VirtualFS(std::string name);
  ~VirtualFS();
  void create_fs_file(unsigned);
  void open_fs_file();
  void upload_file(std::string);
  void download_file(std::string);
  void rename_file(std::string, std::string);
  void remove_file(std::string);
  void list_files();
  void map();
  void open();
  static std::string get_title()
  {
    return std::string
    (
      "\033[1mWirtualny system plikow\n\033[m"
      "Autor: Michal Artur Krawczak\n"
    );
  }
};

#endif