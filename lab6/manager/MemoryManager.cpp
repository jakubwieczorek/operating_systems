//
// Created by jakub on 09.01.18.
//

#include <sstream>
#include <iomanip>
#include <vector>
#include "MemoryManager.h"

bool MemoryManager::createVirtualDisk(string aDiscName, long aDiskSpace)
{
    /*mamy stringa i chcemy z niego zrobic char* to .c_str() trunc zeruje zawartosc podczas zapisu out umozliwia zapis
    in odczyt ate/app zapis na koncu*/
    ofstream file(aDiscName.c_str() + VirtualDisc::DISC_EXTENSION);

    VirtualDisc virtualDisc = VirtualDisc();

    /*trzeba sprawdzic czy pliki zostaly otworzone obie funkcje zwracaja true gdy uzyskalem dostep do pliku.
    zalezy tez od parametrow otwarcia gdy mam dostep tylko do odczytu a zechce pisac to zostanie ustawiona odpowiednia flaga*/
    if(!file.is_open() || !file.good())
    {
        return false;
    }

    file<<aDiscName<<endl
        <<aDiskSpace<<endl
        <<aDiskSpace<<endl
        <<VirtualDisc::SUFFIX<<endl;

    file.close();

    return true;
}

int MemoryManager::copyToDisc(VirtualDisc& aDisc, string aPath)
{
    ifstream file(aPath);

    if(!file.is_open() || !file.good())
    {
        return 3;
    }

    string fileContent((istreambuf_iterator<char>(file)), (istreambuf_iterator<char>()));

    file.close();

    VirtualFile virtualFile = VirtualFile();
    virtualFile.setContent(fileContent);
    virtualFile.setName(aPath);
    virtualFile.setSize(countLines(fileContent) + 4);

    if(aDisc.getFreeSpace() - virtualFile.getSize() < 0)
    {
        return 1;
    }

    for(const auto& el : aDisc.files)
    {
        if(el.getName() == aPath)
        {
            return 2;
        }
    }

    if(aDisc.files.size() != 0)
    {
        for(auto& el : aDisc.files)
        {
            el.setAddress(el.getAddress() + 1); // because line description for new file
        }

        virtualFile.setAddress(aDisc.files.back().getAddress() + aDisc.files.back().getSize());
    } else
    {
        virtualFile.setAddress(5 + 1);
    }

    aDisc.files.push_back(virtualFile);

    aDisc.setFreeSpace(aDisc.getFreeSpace() - countLines(fileContent) - 4);
    save(aDisc);

    return 0;
}

bool MemoryManager::copyFromDisc(VirtualDisc aDisc, string aPath)
{
    return false;
}

VirtualDisc* MemoryManager::openDisc(string aDiscName)
{
    ifstream disc(aDiscName + VirtualDisc::DISC_EXTENSION);

    if(!disc.is_open() || !disc.good())
    {
        return NULL;
    }

    VirtualDisc *virtualDisc = new VirtualDisc();

    string token;

    disc>>token; // disc_name
    virtualDisc->setDiscName(token);
    disc>>token; // disc_space
    virtualDisc->setDiscSize(stol(token));
    disc>>token; // free_size
    virtualDisc->setFreeSpace(stol(token));

    while(disc>>token)
    {
        if(token == VirtualHole::DESCRIPTOR)
        {
            VirtualHole virtualHole = VirtualHole();
            disc>>token; virtualHole.setAddress(stol(token));
            disc>>token; virtualHole.setSize(stol(token));

            virtualDisc->holes.push_back(virtualHole);
        } else if(token == VirtualFile::DESCRIPTOR)
        {
            VirtualFile virtualFile = VirtualFile();
            disc>>token; virtualFile.setAddress(stol(token));
            disc>>token; virtualFile.setSize(stol(token));
            disc>>token; virtualFile.setName(token);

            virtualDisc->files.push_back(virtualFile);
        } else if(token == VirtualFile::PREFIX)// file content
        {
            string name;
            disc>>name; // name

            disc>>token; // ,
            // now only files and maybe holes
            for(list<VirtualFile>::iterator itr= virtualDisc->files.begin();
                itr != virtualDisc->files.end(); ++itr)
            {
                if(itr->getName() == name) // set content in considered file
                {
                    getline(disc, token);
                    while(token != VirtualFile::SUFFIX)
                    {
                        itr->setContent(itr->getContent() + token);
                        getline(disc, token);
                    }
                    break;
                }
            }
        }
    }

    disc.close();

    return virtualDisc;
}

bool MemoryManager::save(VirtualDisc &aDisc)
{
    ofstream disc(aDisc.getDiscName().c_str() + VirtualDisc::DISC_EXTENSION);
    if(!disc.is_open() || !disc.good())
    {
        return false;
    }

    disc<<aDisc.getDiscName()<<endl
        <<aDisc.getDiscSize()<<endl
        <<aDisc.getFreeSpace()<<endl;

    disc<<VirtualDisc::SUFFIX<<endl;

    vector<VirtualFile> files{begin(aDisc.files), end(aDisc.files)};
    vector<VirtualHole> holes{begin(aDisc.holes), end(aDisc.holes)};

    vector<int>::size_type fileIdx = 0;
    vector<int>::size_type holeIdx = 0;
    for(; ;) // petla do wypisywania mapy na poczatku pliku w odpowiedniej kolejnosci
        // nie mozna tylko przejechac po dwoch listach bo adresy nie beda sie zgadzac
    {
        if(fileIdx < files.size() && holeIdx < holes.size())
        {
            if(files[fileIdx].getAddress() < holes[holeIdx].getAddress())
            {
                disc<<files[fileIdx].DESCRIPTOR + " "<<files[fileIdx].getAddress()
                    <<" "<<files[fileIdx].getSize()<<" "<<files[fileIdx].getName()<<endl;

                fileIdx++;
            } else
            {
                disc<<holes[holeIdx].DESCRIPTOR + " "<<holes[holeIdx].getAddress()
                    <<" "<<holes[holeIdx].getSize()<<" "<<endl;

                holeIdx++;
            }
        } else if(fileIdx < files.size() && holeIdx >= holes.size())
        {
            disc<<files[fileIdx].DESCRIPTOR + " "<<files[fileIdx].getAddress()
                <<" "<<files[fileIdx].getSize()<<" "<<files[fileIdx].getName()<<endl;

            fileIdx++;
        } else if(fileIdx >= files.size() && holeIdx < holes.size())
        {
            disc<<holes[holeIdx].DESCRIPTOR + " "<<holes[holeIdx].getAddress()
                <<" "<<holes[holeIdx].getSize()<<" "<<endl;

            holeIdx++;

        } else if(fileIdx >= files.size() && holeIdx >= holes.size())
        {
            break;
        }
    }

    fileIdx = 0;
    holeIdx = 0;
    for(; ;) // to samo co wyzej tylko dla zawartosci i dla dziur, dziura tez jest wypisana
    {
        if(fileIdx < files.size() && holeIdx < holes.size())
        {
            if(files[fileIdx].getAddress() < holes[holeIdx].getAddress())
            {
                disc<<"("<<endl
                    <<files[fileIdx].getName()<<endl
                    <<","<<endl
                    <<files[fileIdx].getContent()<<endl
                    <<")"<<endl;

                fileIdx++;
            } else
            {
                for(long i = 0; i < holes[holeIdx].getSize() + 4; i++)
                {
                    disc<<endl;
                }

                holeIdx++;
            }
        } else if(fileIdx < files.size() && holeIdx >= holes.size())
        {
            disc<<"("<<endl
                <<files[fileIdx].getName()<<endl
                <<","<<endl
                <<files[fileIdx].getContent()<<endl
                <<")"<<endl;

            fileIdx++;
        } else if(fileIdx >= files.size() && holeIdx < holes.size())
        {
            for(long i = 0; i < holes[holeIdx].getSize(); i++)
            {
                disc<<endl;
            }

            holeIdx++;

        } else if(fileIdx >= files.size() && holeIdx >= holes.size())
        {
            break;
        }
    }

    disc.close();

    return false;
}

long MemoryManager::countLines(string aString)
{
    long count = 0;

    for (int i = 0; i < aString.size(); i++)
    {
        if (aString[i] == '\n')
        {
            count++;
        }
    }

    return count + 1;
}

bool MemoryManager::showContent(VirtualDisc aDisc)
{
    for(const auto& el : aDisc.files)
    {
        cout<<el.getName()<<endl;
    }

    return true;
}

bool MemoryManager::showMap(VirtualDisc aDisc)
{
    cout<<"DISC SPACE:   "<<aDisc.getDiscSize()<<endl;
    cout<<"FREE SPACE: "<<aDisc.getFreeSpace()<<endl;

    if(aDisc.files.size() > 0)
    {
        cout<<"FILES:"<<endl;
        cout<<setw(20)<<"FILE"<<setw(10)<<"ADDRESS"<<setw(10)<<"SIZE"<<endl;
        cout<<"----------------------------------------"<<endl;
        for(const auto& el : aDisc.files)
        {
            cout<<setw(20)<<el.getName()<<setw(10)<<el.getAddress()<<setw(10)<<el.getSize()<<endl;
        }
    }

    if(aDisc.holes.size() > 0)
    {
        cout<<"HOLES:"<<endl;
        cout << setw(30) << "ADDRESS" << setw(10) << "SIZE" << endl;
        cout << "----------------------------------------" << endl;
        for (const auto &el : aDisc.holes) {
            cout << setw(30) << el.getAddress() << setw(10) << el.getSize() << endl;
        }
    }

    return false;
}

bool MemoryManager::deleteFileFromDisc(VirtualDisc aDisc, string aFileName)
{
    if(aDisc.files.size() != 0)
    {
        for(auto& el : aDisc.files)
        {
            if(el.getName() == aFileName)
            {
                VirtualHole virtualHole = VirtualHole();
                virtualHole.setAddress(el.getAddress());
                virtualHole.setSize(el.getSize());
                aDisc.holes.push_back(virtualHole);

                // aDisc.setFreeSpace(aDisc.getFreeSpace() + el.getSize()); when defragmentation
                aDisc.files.remove(el);

                break;
            }
        }
    } else
    {
        return false;
    }

    save(aDisc);

    return true;
}

bool MemoryManager::deleteDisc(string aDiscName)
{
    string sDisk = aDiscName + VirtualDisc::DISC_EXTENSION;
    return (bool) remove(sDisk.c_str());
}

bool MemoryManager::copyFileToOs(VirtualDisc aDisc, string aFileName)
{
    for(auto& el : aDisc.files)
    {
        if(el.getName() == aFileName)
        {
            ofstream disc(aFileName);

            if(!disc.is_open() || !disc.good())
            {
                return true;
            }

            disc<<el.getContent();

            disc.close();
        }
    }

    return false;
}

