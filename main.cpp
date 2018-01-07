#include "buffer.h"
#include "docente.h"
#include "producao.h"
#include <string.h>
#include <iostream>
#include <stdlib.h>
#include <fstream>

using namespace std;

int main (int argv, char* argc[]){
    int numeroDeArqRegras = 0;
    char** regrasNomeArquivo = NULL;
    int* anoEntrada = NULL;
    int* anoSaida = NULL;
    int opcao = 0;

    do{
      system("clear");
      cout << "==================BAKER==================" << endl;
      cout << "[1] Importar arquivo de regras           " << endl;
      cout << "[2] Sair                                 " << endl;
      cout << "=========================================" << endl;
      cout << "Opcao: ";
      cin >> opcao;


      switch (opcao) {
        case 1:{
          system("clear");
          int achouArquivo = 1;
          cout << "=============================BAKER=========================================" << endl;
          cout << "Quantos arquivos de regras devem ser importados? (0 = sair)" << endl;
          do{
            int input;
            do{
              cout << "Numero: ";
              cin >> input;
            }while(input < 0);
            if(input == 0 )
              break;
            if(numeroDeArqRegras > 0){
              for(int i=0; i< numeroDeArqRegras;i++){
                delete[]  regrasNomeArquivo[i];
              }
              delete[] regrasNomeArquivo;
            }

            regrasNomeArquivo = new char*[input];
            anoEntrada = new int[input];
            anoSaida = new int[input];

            for(int i=0;i<input;i++){
              regrasNomeArquivo[i] = new char[1024];
              cout << "Nome do arquivo " << (i+1) << " :";
              char* buffer = new char[1024];
              cin >> buffer;
              cout << "Filtro (Menor ano)" << (i+1) << " :";
              int menor;
              cin >> menor;
              cout << "Filtro (Maior ano)" << (i+1) << " :";
              int maior;
              cin >> maior;

              ifstream arquivo;
              arquivo.open(buffer);
              if(arquivo.is_open()){
                cout << "Arquivo de regras " << buffer << " encontrado!" << endl;
                strcpy(regrasNomeArquivo[i], buffer);
                anoEntrada[i] = menor;
                anoSaida[i] = maior;
                arquivo.close();
              }else{
                cout << "Arquivo de regras " << buffer << " NÃO foi encontrado!" << endl;
                achouArquivo = 0;
              }
              delete[] buffer;
            }
            if(achouArquivo != 0)
              numeroDeArqRegras = input;

          }while(achouArquivo != 1);
        }
      }
      cin.ignore();
    }while(opcao != 2 && numeroDeArqRegras < 1);

    for(int i=0;i<numeroDeArqRegras;i++){
      //Create a double linked list of all "docentes"
      docente* docentes = loadAllDocentes("csv/docentes.csv");

      //A binary search tree for producoes (key = docenteId/ Value = list of producao)
      producao* producoes = loadAllProducoes("csv/producao_V2.csv");


      //Create an array with all capes values
      int* rules = loadAllRules(regrasNomeArquivo[i],22);

      //Buffer files contain all valid "capes" and "orientações"
      character* orientacoes = createBufferFile("csv/orientacao.csv");
      character* congressos = createBufferFile("csv/qualis_capes_congressos.csv");
      character* periodicos = createBufferFile("csv/qualis_capes_periodicos.csv");
      //Remove the headers
      removeFirstBufferLine(&orientacoes);
      removeFirstBufferLine(&congressos);


      //Load "area de avaliacao"
      //const char* regrasCaminho = "regraComp";
      char* areaAvaliacao = loadAreaAvaliacao(regrasNomeArquivo[i]);

      //Will fill all give a pontuation to all variables of type "docente"
      baker(&docentes, &producoes, rules, &orientacoes, congressos, periodicos, areaAvaliacao,"csv/orientacao.csv",anoEntrada[i],anoSaida[i]);
      docente* ranking = orderDocentes(docentes);
      ofstream saidaCSV(areaAvaliacao);
      while(ranking->next != NULL){
         saidaCSV << ranking->name << endl;
         cout << "("<< ranking->id << ") " << ranking->name << endl << " pontos:" << ranking->totalPoints << endl;

         for(int i=0;i<22;i++){
           saidaCSV << ranking->points[i];
           if(i != 21)
            saidaCSV << ',';
           cout << i << ": " << ranking->points[i] << endl;
         }
         saidaCSV << endl;
         saidaCSV << ranking->totalPoints << endl;
         ranking = ranking->next;
       }

      cout << "("<< ranking->id << ") " << ranking->name << endl << " pontos:" << ranking->totalPoints << endl;
      saidaCSV << ranking->name << endl;
      for(int i=0;i<22;i++){
        saidaCSV << ranking->points[i];
        if(i != 21)
         saidaCSV << ',';
        cout << i << ": " << ranking->points[i] << endl;
      }
      saidaCSV << ranking->totalPoints << endl;
      saidaCSV.close();

      destroyBufferFile(&orientacoes);
      destroyBufferFile(&congressos);
      destroyBufferFile(&periodicos);
      delete[] rules;
    }
    for(int i =0; i<numeroDeArqRegras;i++)
      delete[] regrasNomeArquivo;
    delete[] anoEntrada;
    delete[] anoSaida;



  return 0;
}
