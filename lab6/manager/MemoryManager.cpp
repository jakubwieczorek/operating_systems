//
// Created by jakub on 09.01.18.
//

#include <sstream>
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
        <<VirtualDisc::MAX_DISC_SIZE<<endl
        <<VirtualDisc::MAX_DISC_SIZE<<endl
        <<VirtualDisc::SUFFIX<<endl;

    file.close();

    return true;
}

bool MemoryManager::copyToDisc(VirtualDisc& aDisc, string aPath)
{
    ifstream file(aPath);

    if(!file.is_open() || !file.good())
    {
        return false;
    }

    string fileContent((istreambuf_iterator<char>(file)), (istreambuf_iterator<char>()));

    file.close();

    VirtualFile virtualFile = VirtualFile();
    virtualFile.setContent(fileContent);
    virtualFile.setName(aPath);
    virtualFile.setSize(fileContent.size());
    virtualFile.setAddress(aDisc.files.back().getAddress() + aDisc.files.back().getSize() + 1);

    //aDisc.files.push_back(virtualFile);
    aDisc.files.insert(aDisc.files.begin(), virtualFile);
    save(aDisc);

    aDisc.setFreeSpace(aDisc.getFreeSpace() - fileContent.size());

    return false;
}

bool MemoryManager::copyFromDisc(VirtualDisc aDisc, string aPath) {
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
    disc>>token; // max_size
    disc>>token; // free_size
    virtualDisc->setFreeSpace(stol(token));

    while(disc>>token)
    {
        if(token == VirtualFile::DESCRIPTOR)
        {
            VirtualHole virtualHole = VirtualHole();
            disc>>token; virtualHole.setAddress(stol(token));
            disc>>token; virtualHole.setSize(stol(token));

            virtualDisc->holes.push_back(virtualHole);
        } else if(token == VirtualHole::DESCRIPTOR)
        {
            VirtualFile virtualFile = VirtualFile();
            disc>>token; virtualFile.setAddress(stol(token));
            disc>>token; virtualFile.setSize(stol(token));
            disc>>token; virtualFile.setName(token);

            virtualDisc->files.push_back(virtualFile);
        } else if(token == VirtualFile::PREFIX)// file content
        {
            disc>>token; // name

            for(list<VirtualFile>::iterator itr= virtualDisc->files.begin();
                itr != virtualDisc->files.end(); ++itr)
            {
                if(itr->getName() == token)
                {
                    while(token != VirtualFile::SUFFIX)
                    {
                        getline(disc, token);
                        itr->setContent(token);
                    }
                    break;
                }
            }
        } else if(token == VirtualDisc::SUFFIX)
        {
            break;
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
        <<aDisc.MAX_DISC_SIZE<<endl
        <<aDisc.getFreeSpace()<<endl;

    disc<<VirtualDisc::SUFFIX<<endl;

    for(list<VirtualFile>::iterator itr = aDisc.files.begin();
        itr != aDisc.files.end(); ++itr)
    {
        disc<<itr->DESCRIPTOR + " "<<itr->getAddress() + " "<<itr->getSize() + " "<<itr->getName()<<endl;
    }

    for(list<VirtualHole>::iterator itr = aDisc.holes.begin();
        itr != aDisc.holes.end(); ++itr)
    {
        disc<<itr->DESCRIPTOR + " "<<itr->getAddress() + " "<<itr->getSize() + " "<<endl;
    }

    for(list<VirtualFile>::iterator itr = aDisc.files.begin();
        itr != aDisc.files.end(); ++itr)
    {
        disc<<"("<<endl
            <<itr->getName()<<endl
            <<","<<endl
            <<itr->getContent()<<endl
            <<")"<<endl;
    }

    disc.close();

    return false;
}
