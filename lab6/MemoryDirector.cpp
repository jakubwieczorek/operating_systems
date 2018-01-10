#include<iostream>

class MemoryDirector
{


public:
	void createVirtualDisk(string aFileName)
	{
		
	/*mamy stringa i chcemy z niego zrobic napis czyli char* to .c_str() trunc zeruje zawartosc podczas zapisu out umozliwia zapis
	in odczyt ate/app zapis na koncu*/
	ofstream file(aFileName.c_str(), ios::trunc | ios::out);
	
	/*trzeba sprawdzic czy pliki zostaly otworzone obie funkcje zwracaja true gdy uzyskalem dostep do pliku.
	zalezy tez od parametrow otwarcia gdy mam dostep tylko do odczytu a zechce pisac to zostanie ustawiona odpowiednia flaga*/
	if(file.is_open()==false || file.good()==false) 
	{
		throw Exception("");
	}

//	for(int i=0; el[i]!=NULL; i++) {if((sumax=el[i]->width+el[i]->local[0])>maxx) maxx=sumax;
//	if((sumay=el[i]->local[1]+el[i]->width)>maxy) maxy=sumay;}
//	maxx+=100;
//	maxy+=100;
//	file<<"<svg width=\""<<maxx<<"\"  height=\""<<maxy<<"\" xmlns=\"http://www.w3.org/2000/svg\">"<<endl;
	file<<"<svg"<<" xmlns=\"http://www.w3.org/2000/svg\">"<<endl;

	for(int i=0; el[i]!=NULL; i++) el[i]->draw(file);//petla po wszystkich elemntach

	file<<"</svg>";
	
	file.close();
	
	return true;
	}
};
