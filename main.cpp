//g++ -std=c++11 main.cpp -lboost_regex
#include <iterator>
#include <algorithm>
#include <iostream>
#include <string>
#include <regex>
#include <list>
#include <unordered_map>
#include <map>
#include <fstream>
#include <sstream>
#include <utility>
#include <stack>

using namespace std;
// lista de simbolos

struct  Simbolo
{
  Simbolo()
  {
    tipo=direccion=valor="none";
    num_parametros = 0;
  }
  string tipo;
  string direccion;
  string valor;
  int num_parametros;
  vector<string> tipo_parametros;
  vector<string> valor_parametros;
  void print()
  {
    cout<<"TIPO:"<<tipo <<"   ";
    cout<<"DIRECCION:"<<direccion<<"   ";
    cout<<"VALOR:"<<valor<<"   ";
    cout<<"NUMEROS DE PARAMETROS:"<<num_parametros<<"   ";
  }

};
vector<string> simbolos_terminales= {"main", ")", "[", "]", ";", "+", "-", "*", "/", "\%", "++", "--", ">", ">=", "<", "<=", "==", "!=", "&&", "||", "int", "double", "string", "char", "void", "cout", "<<", "cin",">>", "variable", "valor_entero", "valor_cadena", "valor_caracter", "valor_double", "valor_bool", "valor_void", "=", "if", "else", "elif", "while", "for", ",", "return", "break", "{", "}", "$"};
vector<string> simbolos_no_terminales = {"TIPO_DATO", "FUNCION", "PARAMETROS", "TIPO_FUNCION", "L", "LISTA_SENTENCIAS", "SENTENCIA", "OPERADORES", "OPERADORES_COMPARACION", "SIMBOLOS_COMPARACION", "OUTPUT", "INPUT", "I", "LINEA_SIMPLE", "DECLARACION", "DECLARACION_ASIGNACION", "NOMBRE_VARIABLE", "S", "ASIGNACION", "TIPO_ASIGNACION", "P", "N", "VALORES", "VALOR_ARRAY", "V", "OPERADOR", "EXPRESION", "TIPO_FACTOR", "E", "IF_ELSE", "IF", "ELSE", "ELIF", "WHILE", "FOR", "DECLARACION_FOR", "CONDICION", "C", "PALABRAS_RESERVADAS", "RETURN", "DATA", "LLAMADA_PARAMETROS", "MAS_PARAMETROS"};
map<string, Simbolo> tabla_simbolos;


unordered_map<string,string> simbolos {

  { "([a-z]+)([a-z|_|0-9])*" , "variable" },
  { "return", "return"},
  { "break", "break"},
  { "main", "main"},
  { "if", "if"},
  { "elif", "elif"},
  { "else", "else"},
  { "while", "while"},
  { "for", "for"},
  { "vector", "vector"},
  { "int", "int"},
  { "double", "double"},
  { "char", "char"},
  { "bool", "bool"},
  { "true", "true"},
  { "false", "false"},
  { "string", "string"},
  { "cout", "cout"},
  { "cin", "cin"},


  { "(\\+|\\-)?[0-9]+\\.[0-9]+" ,   "valor_double" },
  { "(\\+|\\-)?[0-9]+" ,   "valor_entero" },
  { "'.'", "valor_char"},

  { "\"([^\"]*)\"", "valor_cadena"},

  { "\\[", "["},
  { "\\]", "]"},
  { "\\{", "{"},
  { "\\}", "}"},
  { "\\(", "("},
  { "\\)", ")"},
  { "\\,", ","},
  { "\\;", ";"},
  { "\\=", "="},
  { "\\+", "+"},
  { "\\-", "-"},
  { "\\*", "*"},
  { "\\/", "/"},
  { "\\%", "%"},
  { "<<", "<<"},
  { ">>", ">>"},
  { ">", ">"},
  { "<", "<"},
  { ">=", ">="},
  { "<=", "<="},
  { "\\&\\&", "&&"},
  { "\\|\\|", "||"},          
  { "\\!\\=", "!="},
  { "\\=\\=", "=="},
  { "\\+\\+", "++"},
  { "\\-\\-", "--"},


};

ifstream file("my_program.txt");

//<Categoria lexica: <lexema, #linea>>
vector<pair<string,pair<string, int> > > lista_separadores;
vector<string> errores;
int num_linea = 1;

string configure_line(string line)
{
  string temp;
  regex empty_spaces ("( |\t)+");
  regex_replace (back_inserter(temp), line.begin(), line.end(), empty_spaces, " ");    

  line = temp;
  if(line.size() and line.at(0)==' ')
    line = line.substr(1);

    string items = "=+-*/<>;)({}[],!";
  string salida = "";
  int pos;
  for (int j = 0; j < line.size(); j++)
  {
    for(int i = 0 ; i < items.size(); i++)
    {
      if(line[j] == '(' and (line[j+1] == '+' or line[j+1]=='-'))
      {

        salida = salida + " " + line[j]+ " " + line[j+1];
        j=j+2;
      }
      else
      {
        if(line[j] == '=' and (line[j+1] == '+' or line[j+1]=='-'))
        {
          salida = salida + " " + line[j] + " " + line[j+1];
          j=j+2;
        }
        else
        {
          if(line [j] == items[i])
          {
            if(line[j+1] == '=' or line[j+1] == '<' or line[j+1] == '>' or line[j+1] == '+' or line[j+1] == '-' or line[j+1] == '!')
            {
              salida = salida + " "+ line[j]+line[j+1]+ " ";
              j=j+2;
            }
            else
            {
              if(line.size()==1)
                continue;
              else if(j+1==line.size())
                salida = salida + " "+ line[j];
              else
                salida = salida + " "+ line[j]+ " ";
              j++;    
            }
          }

        }

      }


    }
    if(line[j])
      salida = salida + line[j];    
  }

  return salida;
}
string quitar_espacios(string s)
{
  string salida="";
  for(int i=0; i<s.size();i++)
  {
    if(s[i]==' ' or s[i]=='\t')
      continue;
    else
      salida+=s[i];
  }
  return salida;
}

void analizador_lexico(string str)
{ 
  bool ac=true;

  for(auto & symbol : simbolos)
  {
    regex tmp(symbol.first);
    if(str == " " or str =="\t")
      break;
    else
    {
      if(regex_match (str, tmp))
      {
        ac=false;
        lista_separadores.push_back(make_pair(symbol.second, make_pair(str, num_linea)));
        break;
      }
    } 
  }

  if(ac)
  {
    if(str.size()>1)
      errores.push_back("Invalid Token '"+str+"' en la linea "+to_string(num_linea));
  }

}
map<pair<string, string> , vector<string>  >   tabla;
void llenar_tabla()
{
  ifstream file;
  file.open("tabla");
    //PROGRAMA return RETRUN SENTENCIA

  string linea;

  string uno, dos, tres, cuatro;
  while(getline (file, linea))
  {
    cuatro = "";
    stringstream ss;
    ss << linea;

    ss >> uno;
    ss >> dos;

    tabla[make_pair(uno,dos)];
    while(ss >> tres)
    {            
      tabla[make_pair(uno,dos)].push_back(tres);
    }

  }

}
struct No_terminal
{
  No_terminal()
  {
    simbolo=tipo=valor=lex="none";
  }
  string simbolo;
  string lex;
  string tipo;
  string valor;
};
vector<No_terminal> nt;
int encontrar_simbolo(string simbolo, int n=1)
{
  int t=1;
  for(int i=nt.size()-1; i>=0; i--)
  {
    if(nt[i].simbolo==simbolo and t++==n)
      return i;
  }
  return -1;
}
void hacer_regla(string regla)
{
  if(regla == "_R1")
  {
    int indice = encontrar_simbolo("TIPO_DATO");
    nt[indice].tipo = "int";
  }
  else if(regla == "_R2")
  {
    int indice = encontrar_simbolo("TIPO_DATO");
    nt[indice].tipo = "double";
  }
  else if(regla == "_R3")
  {
    int indice = encontrar_simbolo("TIPO_DATO");
    nt[indice].tipo = "string";
  }
  else if(regla == "_R4")
  {
    int indice = encontrar_simbolo("TIPO_DATO");
    nt[indice].tipo = "char";
  }
  else if(regla == "_R5")
  {
    int indice = encontrar_simbolo("TIPO_DATO");
    nt[indice].tipo = "void";
  }
  else if(regla == "_R6")
  {
    int indice = encontrar_simbolo("NOMBRE_VARIABLE");
    tabla_simbolos[nt[indice].lex].tipo = nt[encontrar_simbolo("TIPO_DATO")].tipo;
    nt[encontrar_simbolo("X")].tipo = nt[encontrar_simbolo("TIPO_DATO")].tipo;
    //tabla_simbolos[nt[indice].lex].valor = nt[encontrar_simbolo("X")].valor;
  }
  else if(regla == "_R7")
  {
    //tabla_simbolos[encontrar_simbolo['variable']].tipo = nt[indice].tipo;
    int indice = encontrar_simbolo("NOMBRE_VARIABLE");
    nt[indice].lex = nt[encontrar_simbolo("variable")].lex;
  }

  else if(regla == "_R8")
  {
    int indice = encontrar_simbolo("NOMBRE_VARIABLE");
    nt[encontrar_simbolo("X")].tipo = nt[encontrar_simbolo("X",2)].tipo;
    tabla_simbolos[nt[indice].lex].tipo = nt[encontrar_simbolo("X")].tipo;
  }

  else if(regla == "_R9")
  {
    int indice = encontrar_simbolo("VALORES");
    nt[indice].valor = nt[encontrar_simbolo("valor_entero")].valor;
    nt[indice].lex = nt[encontrar_simbolo("valor_entero")].lex;
  }
  else if(regla == "_R16")
  {
    int indice = encontrar_simbolo("VALORES");
    nt[indice].valor = nt[encontrar_simbolo("valor_double")].valor;
    nt[indice].lex = nt[encontrar_simbolo("valor_double")].lex;
  }
  else if(regla == "_R17")
  {
    int indice = encontrar_simbolo("VALORES");
    nt[indice].valor = nt[encontrar_simbolo("valor_cadena")].valor;
    nt[indice].lex = nt[encontrar_simbolo("valor_cadena")].lex;
  }
  else if(regla == "_R18")
  {
    int indice = encontrar_simbolo("VALORES");
    nt[indice].valor = nt[encontrar_simbolo("valor_caracter")].valor;
    nt[indice].lex = nt[encontrar_simbolo("valor_caracter")].lex;
  }
  else if(regla == "_R19")
  {
    int indice = encontrar_simbolo("VALORES");
    nt[indice].valor = nt[encontrar_simbolo("valor_bool")].valor;
    nt[indice].lex = nt[encontrar_simbolo("valor_bool")].lex;
  }
  
  else if(regla == "_R10")
  {
    int indice = encontrar_simbolo("TIPO_FACTOR");
    nt[indice].valor = nt[encontrar_simbolo("VALORES")].valor;
    nt[indice].lex = nt[encontrar_simbolo("VALORES")].lex;
  }
  else if(regla == "_R11")
  {
    int indice = encontrar_simbolo("EXPRESION");
    nt[indice].valor = nt[encontrar_simbolo("TIPO_FACTOR")].valor;
    nt[indice].lex = nt[encontrar_simbolo("TIPO_FACTOR")].lex;
  }
  else if(regla == "_R12")
  {
    int indice = encontrar_simbolo("P");
    nt[indice].valor = nt[encontrar_simbolo("EXPRESION")].valor;
    nt[indice].lex = nt[encontrar_simbolo("EXPRESION")].lex;
  }
  else if(regla == "_R13")
  {
    int indice = encontrar_simbolo("DECLARACION_ASIGNACION");
    nt[indice].valor = nt[encontrar_simbolo("P")].valor;
    nt[indice].lex = nt[encontrar_simbolo("P")].lex;
  }
  else if(regla == "_R14")
  {
    int indice = encontrar_simbolo("X");
    nt[indice].valor = nt[encontrar_simbolo("DECLARACION_ASIGNACION")].valor;
    nt[indice].lex = nt[encontrar_simbolo("DECLARACION_ASIGNACION")].lex;
  }
  else if(regla == "_R15")
  {
    int indice = encontrar_simbolo("NOMBRE_VARIABLE");
    if(nt[encontrar_simbolo("X")].lex != "valor_entero")
      cout<<"error"<<endl;
    else
      tabla_simbolos[nt[indice].lex].valor = nt[encontrar_simbolo("X")].valor;  
  }

  else if(regla == "_R22")
  {
    int indice = encontrar_simbolo("NOMBRE_VARIABLE");
    if(nt[encontrar_simbolo("X")].lex != "valor_double")
      cout<<"error"<<endl;
    else
      tabla_simbolos[nt[indice].lex].valor = nt[encontrar_simbolo("X")].valor;  
  }

  else if(regla == "_R23")
  {
    int indice = encontrar_simbolo("NOMBRE_VARIABLE");
    if(nt[encontrar_simbolo("X")].lex != "valor_cadena")
      cout<<"error"<<endl;
    else
      tabla_simbolos[nt[indice].lex].valor = nt[encontrar_simbolo("X")].valor;  
  }

  else if(regla == "_R24")
  {
    int indice = encontrar_simbolo("NOMBRE_VARIABLE");
    if(nt[encontrar_simbolo("X")].lex != "valor_caracter")
      cout<<"error"<<endl;
    else
      tabla_simbolos[nt[indice].lex].valor = nt[encontrar_simbolo("X")].valor;  
  }
  else if(regla == "_R20")
  {
    int indice = encontrar_simbolo("TIPO_ASIGNACION");
    nt[indice].valor = nt[encontrar_simbolo("P")].valor;
    nt[indice].lex = nt[encontrar_simbolo("P")].lex;
  }
  else if(regla == "_R21")
  {
    int indice = encontrar_simbolo("NOMBRE_VARIABLE");
    
    if(tabla_simbolos[nt[indice].lex].tipo == "int" and nt[encontrar_simbolo("TIPO_ASIGNACION")].lex =="valor_entero")
      tabla_simbolos[nt[indice].lex].valor = nt[encontrar_simbolo("TIPO_ASIGNACION")].valor;
    else if(tabla_simbolos[nt[indice].lex].tipo == "double" and nt[encontrar_simbolo("TIPO_ASIGNACION")].lex =="valor_double")
      tabla_simbolos[nt[indice].lex].valor = nt[encontrar_simbolo("TIPO_ASIGNACION")].valor;
    else if(tabla_simbolos[nt[indice].lex].tipo == "string" and nt[encontrar_simbolo("TIPO_ASIGNACION")].lex =="valor_cadena")
      tabla_simbolos[nt[indice].lex].valor = nt[encontrar_simbolo("TIPO_ASIGNACION")].valor;
    else if(tabla_simbolos[nt[indice].lex].tipo == "char" and nt[encontrar_simbolo("TIPO_ASIGNACION")].lex =="valor_caracter")
      tabla_simbolos[nt[indice].lex].valor = nt[encontrar_simbolo("TIPO_ASIGNACION")].valor;
    else cout<<"error"<<endl;

  }
}

bool analizador_sintactico()
{
  llenar_tabla();
  stack<string> pila;
  pila.push("$");
  pila.push("FUNCION");
  for(int i = 0 ; i<lista_separadores.size(); )
  {
        //cout << endl;
        //get_LEXEMA   
    cout << endl;     
    string lexema = lista_separadores[i].first;
    string get_cola = pila.top();
    stack<string> pila_temp(pila);
    vector<string> result_tabla;
    cout<<"COLA: ";
    while(!pila_temp.empty())
    {
      cout<<pila_temp.top()<< "  ";
      pila_temp.pop();
    }
    cout<<endl;
    cout<<"LEXEMA: "<<lexema<<endl;

    cout<<"LEXEMA_valor: "<< lista_separadores[i].second.first<<endl;

    if(get_cola != "$" and get_cola == lexema)
    {
      pila.pop();
      i++;
      continue;
    }
    else
    {
      if(lexema == "$"  and get_cola == "$")
      {
        cout<<"CORRECTO"<<endl;
        return 1;
      }
    }

    if(get_cola[0] == '_')
    {
      cout << "Regla!!"<< endl;
      hacer_regla(get_cola);
      pila.pop();
      continue;
    }

    result_tabla = tabla[make_pair(get_cola, lexema)];
            /*cout<<"vector: ";
            for(auto a: result_tabla)
                cout<<a<<"  ";
            cout << endl;*/

    if(result_tabla.size() == 0)
    {
      cout<<"ERROR"<<endl;
      return 0;
    }   
    else
    {
      pila.pop();
      if(result_tabla[0] == "epsilon")
      {
        continue; 
      }   
      for(int j = result_tabla.size()-1; j >= 0; j--)
      {
        pila.push(result_tabla[j]);       
      }
      for(int j = 0; j < result_tabla.size(); ++j)
      {
        if(result_tabla[j][0]!='_')
        {
          No_terminal a;
          a.simbolo = result_tabla[j];
          if(lista_separadores[i].first=="variable")
          {               
            a.lex = lista_separadores[i].second.first;
            cout << " variable : " << a.lex  << endl;
          }
          else if(lista_separadores[i].first == "valor_entero" or lista_separadores[i].first == "valor_double" or lista_separadores[i].first == "valor_cadena" or lista_separadores[i].first == "valor_caracter" or lista_separadores[i].first == "valor_bool")
          {
            a.valor = lista_separadores[i].second.first;
            a.lex = lista_separadores[i].first;
            cout << " valor : " << a.valor  << endl;
          }

          nt.push_back(a);
          cout<<"añaadiendo.."<<a.simbolo<<endl;  
        }

      }
    }
  }
}

int main(){
    //llenar_tabla();
    //return 1;
  string line; 
  while (getline(file, line))
  {
    if(line.size())
    {
      string line_configured = configure_line(line);
      vector<string> strings;
      istringstream f(line_configured);
      string s;
      string ss;
      bool is_string = false;
      while (getline(f, s, ' ')) {
        if(s.size())
        {
          if(s[0] == '"' )
          {
            if(s[s.size()-1] != '"' or s.size()==1)
            {
              char tmp = 'c';
              string buffer;
              while(f.get(tmp) and tmp != '"')
              {
                buffer += tmp;
              }
              if(tmp == '"')
              {
                buffer+=tmp;
                analizador_lexico(s+" "+buffer);
              }
              else
                cout<<"No se encontro fin de cadena: linea " <<num_linea<<endl;    
            }
            else
              analizador_lexico(s);

          }
          else
            analizador_lexico(s);    
        }

      }

    }
    num_linea++;
  }
  lista_separadores.push_back(make_pair("$", make_pair("$", -1)));

  for(int i = 0 ; i<lista_separadores.size(); i++)
  {
    Simbolo simbolo;
    if(lista_separadores[i].first == "variable")
    {
      tabla_simbolos[lista_separadores[i].second.first];
      tabla_simbolos[lista_separadores[i].second.first] = simbolo;
    }
    cout<<"CATEGORIA LEXICA: "<<lista_separadores[i].first<<endl;
    cout<<"LEXEMA: "<<lista_separadores[i].second.first<<endl;
    cout<<"NUMERO DE LINEA: "<<lista_separadores[i].second.second<<endl;
  }
  for(int i = 0 ; i<errores.size(); i++)
  {
    cout<<errores[i]<<endl;
  }
  bool a = analizador_sintactico();
  cout<<"BOOL:"<<a<<endl;

  for(auto &simbolo:nt)
  {
    cout<<"SIMBOLO:"<<simbolo.simbolo <<" tipo: "<< simbolo.tipo << " valor: " << simbolo.valor << " lex: "<< simbolo.lex << endl;
  }
  cout << endl; 

  for(auto &simbolo:tabla_simbolos)
  {
    cout << endl;
    cout<<"SIMBOLO:"<<simbolo.first<<"   ";
    simbolo.second.print();
  }
  cout << endl; 
}
