#include <iostream>
#include <fstream>
#include <unordered_map>
#include <math.h>
#include <vector>
#include <set>
#include <algorithm>
#include <string>
#include <sstream>
#include <time.h>

using namespace std;

#define time2 double(clock())/CLOCKS_PER_SEC

enum TToken { NONE, KEY, WORD, NUMBER, AUTHOR, SOURCE, TEOF, ERROR };
enum TState { S0, S1, S2, S3, S4, S5, S6, S7};

struct Item {
    string rn;
    double score;
};

vector<Item> sort_ranking(1240);

class Token {
  public:
    TToken tok;
    string lexeme;
    Token () { tok = NONE; lexeme = ""; }
    Token (TToken t, string l) { tok = t; lexeme = l; }
};

class Lexer {
    string buffer, lexema;
    int pos;
    char cchar;
    TState state, pstate;
    ifstream infile;
    void get_char();
    void read_line();
  public:
    void set_file(const char* file_name);
    Token next_token();
    Token next_token2();
};

void Lexer::set_file(const char* file_name) {
  infile.close();
  infile.open(file_name);
  read_line();
  state = S0;
}

void Lexer::read_line() {
  if (getline(infile, buffer)) {
    pos = 0;
    get_char();
  }
  else {
    cchar = EOF;
  }
}

void Lexer::get_char() {
  cchar = buffer[pos];
  pos++;
}

Token Lexer::next_token () {
  string lex = "";
  while (true) {
    if (state == S0) {
      if (cchar >= 'A' && cchar <= 'Z') {
        if (pstate == S6 && lex != "") {
          Token token(SOURCE, lex);
          return token;
        }
        else {
          state = S1;
        }
      }
      else if (cchar == ' ' || cchar == '\t') {
        state = S2;
      }
      else if (cchar == '\0') {
        read_line();
      }
      else if (cchar == EOF) {
        Token token(TEOF, "End of file");
        return token;
      }
      else {
        //Error
        state = pstate;
      }
    }
    else if (state == S1) {
      if (cchar >= 'A' && cchar <= 'Z') {
        lex += cchar;
        get_char();
      }
      else if (cchar == ' ') {
        state = S2;
        Token token(KEY, lex);
        if (lex == "RN" or lex == "AN") { pstate = S3; return token; }
        else if (lex == "AU"){ pstate = S4; return token; }
        else if (lex == "TI" || lex == "MJ" || lex == "MN"
             || lex == "AB" || lex == "EX") { pstate = S5; return token; }
        else if (lex == "SO") { pstate = S6; return token; }
        else { lex = ""; pstate = S7; }
      }
      else {
        //Error
        state = pstate;
      }
    }
    else if (state == S2) {
      if (pstate == S7) {
        read_line();
        state = S0;
      }
      else if (cchar == ' ' || cchar == '\t') {
        get_char();
      }
      else if (cchar == '\0') {
        read_line();
        state = S0;
      }
      else if (cchar >= '0' && cchar <= '9' && pstate == S3) {
        //Number
        state = S3;
      }
      else if ((cchar != ' ') && (cchar != '\t') && (pstate == S6)) {
        if (lex != "") { lex += ' '; }
        state = S6;  //Source
      }
      else if ((cchar >= 'a' && cchar <= 'z') || (cchar >= 'A' && cchar <= 'Z')) {
        if (pstate == S4) { state = S4; } //Author
        else if (pstate == S5) { state = S5; } //Word
        else {
          //Error
          Token token(ERROR, "Error1");
          return token;
        }
      }
      else if ((pstate == S5) && (cchar < 'A' || (cchar > 'Z' && cchar < 'a') || cchar > 'z')) { //Y no es una letra
        get_char();
      }
      else {
        //Error
        Token token(ERROR, "Error2");
        return token;
      }
    }
    else if (state == S3) {
      if (cchar >= '0' && cchar <= '9') {
        lex += cchar;
        get_char();
      }
      else {
        state = S2;
        Token token(NUMBER, lex);
        return token;
      }
    }
    else if (state == S4) {
      if ((cchar >= 'a' && cchar <= 'z') || (cchar >= 'A' && cchar <= 'Z') || (cchar >= '0' && cchar <= '9') || (cchar == '-')) {
        lex += cchar;
        get_char();
      }
      else if (cchar == '.') {
        get_char();
        state = S2;
        Token token(AUTHOR, lex);
        return token;
      }
      else {
        //Error
        Token token(ERROR, "Error3");
        return token;
      }
    }
    else if (state == S5) {
      if ((cchar >= 'a' && cchar <= 'z') || (cchar >= 'A' && cchar <= 'Z')) {
        lex += cchar;
        get_char();
      }
      else if (lex == "") {
        state = S2;
      }
      else {
        state = S2;
        Token token(WORD, lex);
        return token;
      }
    }
    else if (state == S6) {
      if (cchar != '\0' ) {
        lex += cchar;
        get_char();
      }
      else {
        read_line();
        pstate = S6;
        state = S0;
      }
    }
    else {
      //Error
      Token token(ERROR, "Error");
      return token;
    }
  }
}

Token Lexer::next_token2 () {
  string lex = "";
  while (true) {
    if (state == S0) {
      if (cchar >= 'A' && cchar <= 'Z') {
        state = S1;
      }
      else if (cchar == ' ' || cchar == '\t') {
        state = S2;
      }
      else if (cchar == '\0') {
        read_line();
      }
      else if (cchar == EOF) {
        Token token(TEOF, "End of file");
        return token;
      }
      else {
        //Error
        state = pstate;
      }
    }
    else if (state == S1) {
      if (cchar >= 'A' && cchar <= 'Z') {
        lex += cchar;
        get_char();
      }
      else if (cchar == ' ') {
        state = S2;
        Token token(KEY, lex);

        if (lex == "QN") { pstate = S3; return token; }
        else if (lex == "QU"){ pstate = S5; return token; }
        else if (lex == "RD") { pstate = S3; return token; }
        else { lex = ""; pstate = S7; }
      }
      else {
        //Error
        state = pstate;
      }
    }
    else if (state == S2) {
      if (pstate == S7) {
        read_line();
        state = S0;
      }
      else if (cchar == ' ' || cchar == '\t') {
        get_char();
      }
      else if (cchar == '\0') {
        read_line();
        state = S0;
      }
      else if (cchar >= '0' && cchar <= '9' && pstate == S3) {
        //Number
        state = S3;
      }
      else if ((cchar >= 'a' && cchar <= 'z') || (cchar >= 'A' && cchar <= 'Z')) {
        state = S5;
      }
      else if ((pstate == S5) && (cchar < 'A' || (cchar > 'Z' && cchar < 'a') || cchar > 'z')) { //Y no es una letra
        get_char();
      }
      else {
        //Error
        Token token(ERROR, "Error2");
        return token;
      }
    }
    else if (state == S3) {
      if (cchar >= '0' && cchar <= '9') {
        lex += cchar;
        get_char();
      }
      else {
        state = S2;
        Token token(NUMBER, lex);
        return token;
      }
    }
    else if (state == S5) {
      if ((cchar >= 'a' && cchar <= 'z') || (cchar >= 'A' && cchar <= 'Z')) {
        lex += cchar;
        get_char();
      }
      else if (lex == "") {
        state = S2;
      }
      else {
        state = S2;
        Token token(WORD, lex);
        return token;
      }
    }
    else {
      //Error
      Token token(ERROR, "Error");
      return token;
    }
  }
}

//Dictionary

struct Document {
    int frec;
    double tf;
    string doc;
    Document *next;
};

struct DictElement {
    int frec;
    double idf;
    Document *first;
};

struct Document_tmp {
    string rn;
    unordered_map <string, int> words;
};

class Dictionary {
    int total_docs;
    string actual_doc_rn;
    unordered_map <string, DictElement> dict;
    unordered_map <string, int> stopwords;
    unordered_map <string, double> docs_norm;
  public:
    Dictionary (const char* stopwords);
    void load_stopwords(const char* file_name);
    string pre_processing(string word);
    void create_document(string rn);
    void close_document();
    void add_to_document(string word);
    void calc_tf_idf();
    int pross_query(vector<string> query);
};

Dictionary::Dictionary (const char* stopwords) {
  total_docs = 0;
  load_stopwords(stopwords);
}

int Dictionary::pross_query(vector<string> query){
  unordered_map <string, double> ranking;
  double idf_tmp;

  for ( int w = 0; w < query.size(); w++ ){
    string keyword = pre_processing(query[w]);
    if (keyword != ""){
      unordered_map <string, DictElement>::const_iterator got_w = dict.find(keyword);
      if (got_w != dict.end()){
        Document* pDocument = got_w->second.first;
        idf_tmp = got_w->second.idf;
        while (pDocument){
          unordered_map <string, double>::const_iterator got_d = ranking.find(pDocument->doc);
          if (got_d != ranking.end()){
            ranking[pDocument->doc] += pDocument->tf * idf_tmp * idf_tmp;
          } else {
            ranking[pDocument->doc] = pDocument->tf * idf_tmp * idf_tmp;
          }
          pDocument = pDocument->next;
        }
      }
    }
  }
  int index = 0;
  for ( auto it = ranking.begin(); it != ranking.end(); ++it ) {
    it->second = it->second / docs_norm[it->first];
    sort_ranking[index].rn = it->first;
    sort_ranking[index].score = it->second;
    index++;
  }
  sort( sort_ranking.begin(), sort_ranking.begin() + index - 1, []( const Item lhs, const Item rhs )
           { return lhs.score > rhs.score; } );
  return index;
}

void Dictionary::load_stopwords(const char* file_name) {
  ifstream infile;
  string buffer;
  infile.open(file_name);
  while (getline(infile, buffer)) {
    stopwords.emplace(buffer, 1);
  }
}

string Dictionary::pre_processing(string word) {
  string new_word = "";
  if (word.length() <= 2){
    return new_word;
  }
  for(int c=0; word[c] != '\0'; c++){
    if (word[c] >= 'A' && word[c] <= 'Z'){
      new_word += word[c] + 32;
    }
    else {
      new_word += word[c];
    }
  }
  //Stopwords
  unordered_map <string, int>::const_iterator got = stopwords.find(new_word);
  if (got != stopwords.end()){
    new_word = "";
  }
  return new_word;
}

void Dictionary::create_document(string rn) {
  actual_doc_rn = rn;
  total_docs++;
}

void Dictionary::close_document() {
  actual_doc_rn = "";
}

void Dictionary::calc_tf_idf() {
  double idf_tmp;
  for ( auto it = dict.begin(); it != dict.end(); ++it ){
    it->second.idf = log2(double(total_docs) / double(it->second.frec));
    idf_tmp = it->second.idf;
    Document* pDocument = it->second.first;
    while (pDocument){
      pDocument->tf = 1 + log2(pDocument->frec);
      unordered_map <string, double>::const_iterator got_d = docs_norm.find(pDocument->doc);
      if (got_d != docs_norm.end()){
        docs_norm[pDocument->doc] += pDocument->tf * idf_tmp * pDocument->tf * idf_tmp;
      } else {
        docs_norm[pDocument->doc] = pDocument->tf * idf_tmp * pDocument->tf * idf_tmp;
      }
      pDocument = pDocument->next;
    }
  }
  for ( auto it = docs_norm.begin(); it != docs_norm.end(); ++it ){
    it->second = sqrt(it->second);
  }
}

void Dictionary::add_to_document(string word) {
  string correct_word = pre_processing(word);
  if (correct_word != ""){
    unordered_map <string, DictElement>::const_iterator got = dict.find(correct_word);
    if (got == dict.end()){
      //Crear palabra de vocabulario
      DictElement* pDictElement = new DictElement();
      pDictElement->frec = 1;
      pDictElement->first = NULL;
      //Crear documento
      Document* pDocument = new Document();
      pDocument->doc = actual_doc_rn;
      pDocument->frec = 1;
      pDocument->next = pDictElement->first;
      pDictElement->first = pDocument;
      dict.emplace(correct_word, (*pDictElement));
    }
    else {
      if (got->second.first->doc == actual_doc_rn){
        got->second.first->frec++;
      }
      else {
        //Crear documento
        Document* pDocument = new Document();
        pDocument->doc = actual_doc_rn;
        pDocument->frec = 1;
        pDocument->next = got->second.first;
        dict[correct_word].first = pDocument;
        dict[correct_word].frec++;
      }
    }
  }
}

class Parser {
    Token token;
    Lexer* lexer;
    Dictionary* dict;
  public:
    void DO();
    void AR();
    void RN();
    void AN();
    void AU();
    void AU2();
    void TI();
    void TI_WORD();
    void SO();
    void MJ();
    void MJ_WORD();
    void MN();
    void MN_WORD();
    void AB();
    void AB_WORD();
    void EX();
    Parser (Lexer* lex, Dictionary* d);
    void reconoce();
    void procesa();
    void error();
};

Parser::Parser (Lexer* lex, Dictionary* d) {
  lexer = lex;
  dict = d;
}

void Parser::reconoce() {
  token = lexer->next_token();
}

void Parser::procesa() {
  dict->add_to_document(token.lexeme);
  token = lexer->next_token();
}

void Parser::error() {
  cout << "Error reading the file..." << endl;
  for(;;){}
}

void Parser::DO() {
  token = lexer->next_token();
  while (token.tok != TEOF) {
    AR();
  }
  reconoce();
}

void Parser::AR() {
  if (token.tok == KEY && token.lexeme == "RN") {
    reconoce();
    RN();
  } else { error();}
}

void Parser::RN() {
  if (token.tok == NUMBER) {
    dict->create_document(token.lexeme);
    reconoce();
    AN();
  } else { error();}
}

void Parser::AN() {
  if (token.tok == KEY && token.lexeme == "AN") {
    reconoce();
    reconoce();
    AU();
  } else { error();}
}

void Parser::AU() {
  if (token.tok == KEY && token.lexeme == "AU") {
    reconoce();
    AU2();
  } else { TI();}
}

void Parser::AU2() {
  if (token.tok == AUTHOR) {
    reconoce();
    AU2();
  } else {
    TI();
  }
}

void Parser::TI() {
  if (token.tok == KEY && token.lexeme == "TI") {
    reconoce();
    TI_WORD();
  } else { error();}
}

void Parser::TI_WORD() {
  if (token.tok == WORD) {
    procesa();
    TI_WORD();
  } else {
    SO();
  }
}

void Parser::SO() {
  if (token.tok == KEY && token.lexeme == "SO") {
    reconoce();
    reconoce();
    MJ();
  } else {
    error();
  }
}

void Parser::MJ() {
  if (token.tok == KEY && token.lexeme == "MJ") {
    reconoce();
    MJ_WORD();
  } else if (token.tok == KEY && token.lexeme == "MN") {
      MN();
  } else { error();}
}

void Parser::MJ_WORD() {
  if (token.tok == WORD) {
    procesa();
    MJ_WORD();
  } else {
    MN();
  }
}

void Parser::MN() {
  if (token.tok == KEY && token.lexeme == "MN") {
    reconoce();
    MN_WORD();
  } else { error();}
}

void Parser::MN_WORD() {
  if (token.tok == WORD) {
    procesa();
    MN_WORD();
  } else if ((token.tok == KEY) && (token.lexeme == "AB" || token.lexeme == "EX")){
      AB();
  } else { error();}
}

void Parser::AB() {
  if (token.tok == KEY && (token.lexeme == "AB" || token.lexeme == "EX")) {
    reconoce();
    AB_WORD();
  } else { error();}
}

void Parser::AB_WORD() {
  if (token.tok == WORD) {
    procesa();
    AB_WORD();
  }
}

int main()
{
    double a, total = 0;
    ofstream results_file;
    results_file.open ("results.txt");

    cout << "    =====================================================" << endl;
    cout << "    ==  Trabalho Pratico de Recuperacao de Informacao  ==" << endl;
    cout << "    ==  Modelo vetorial                                ==" << endl;
    cout << "    ==  Aluno: Roland Rodriguez Romero                 ==" << endl;
    cout << "    ==  Matricula: 2150310                             ==" << endl;
    cout << "    =====================================================" << endl;
    cout << endl;

    results_file << "    =====================================================" << endl;
    results_file << "    ==  Trabalho Pratico de Recuperacao de Informacao  ==" << endl;
    results_file << "    ==  Modelo vetorial                                ==" << endl;
    results_file << "    ==  Aluno: Roland Rodriguez Romero                 ==" << endl;
    results_file << "    ==  Matricula: 2150310                             ==" << endl;
    results_file << "    =====================================================" << endl;
    results_file << endl;

    Token tok;
    Lexer lexer;
    lexer.set_file("cfquery");

    struct Query {
        vector<string> words;
        set<int> relevants_list;
    };
    Query querys[101];

    int q = 0, n, flag;
    tok = lexer.next_token2();

    while (tok.tok != TEOF){
        if (tok.tok == KEY && tok.lexeme == "QN"){
            tok = lexer.next_token2();
            if (tok.tok == NUMBER){
                stringstream stringtoint;
                stringtoint << tok.lexeme;
                stringtoint >> q;
            }
        }
        else if (tok.tok == KEY && tok.lexeme == "RD"){
            flag = 1;
        }
        else if (tok.tok == WORD){
            querys[q].words.push_back(tok.lexeme);
        }
        else if (tok.tok == NUMBER){
            if (flag == 1){
                stringstream stringtoint;
                stringtoint << tok.lexeme;
                stringtoint >> n;
                querys[q].relevants_list.insert(n);
                flag = 0;
            }
            else {flag = 1;}
        }
        tok = lexer.next_token2();
    }

    Dictionary dict("stopwords.txt");
    Parser parser(&lexer, &dict);
    lexer.set_file("cf74");
    parser.DO();
    lexer.set_file("cf75");
    parser.DO();
    lexer.set_file("cf76");
    parser.DO();
    lexer.set_file("cf77");
    parser.DO();
    lexer.set_file("cf78");
    parser.DO();
    lexer.set_file("cf79");
    parser.DO();
    dict.calc_tf_idf();

    unordered_map <string, int> art[100];

    int ranking_size;
    double p10_total = 0, mapr_total = 0;

    for ( int q = 1; q < 101; q++ ){
        //Imprimir relevantes
        results_file << "===================== Query " << q << " ==============================================" << endl;
        cout << "--------------------------------------------------------------------------" << endl;
        results_file << endl;
        results_file << "Relevant docs: ";
        for ( auto it = querys[q].relevants_list.begin(); it != querys[q].relevants_list.end(); ++it ){
          results_file << *it << " ";
        }
        a=time2;
        ranking_size = dict.pross_query(querys[q].words);
        total += time2-a;
        //Calcular P@10 y MAP
        results_file << endl;
        results_file << endl;
        results_file << "Results: " ;

        int docs_count = 0, rel_count = 0, doc;
        double prec = 0, prec_sum = 0, p10, mapr;
        for(int i = 0; i < ranking_size; i++){
            docs_count++;
            stringstream stringtoint;
            stringtoint << sort_ranking[i].rn;
            stringtoint >> doc;
            set <int>::const_iterator got_d = querys[q].relevants_list.find(doc);

            if (got_d != querys[q].relevants_list.end()){
                rel_count++;
                prec = double(rel_count) / double(docs_count);
                prec_sum += prec;
                results_file << "(" << doc << ") " ;
            } else {
                results_file << doc << " " ;
            }
            if (docs_count == 10){
              p10 = double(rel_count) / 10;
              p10_total += p10;
            }
        }
        results_file << endl;
        results_file << endl;
        mapr = prec_sum / rel_count;
        mapr_total += mapr;
        cout << " Query " << q << " =>   MAP: " << mapr << "    P@10: " << p10 << "    " << rel_count << " relevant of "<< ranking_size << " results. " << endl;
        results_file << "    MAP: " << mapr << "    P@10: " << p10 << "    " << rel_count << " relevant of "<< ranking_size << " results. " << endl;
        results_file << "   ---------------------------------------------------------------" << endl;
        results_file << endl;
        results_file << endl;
    }
    cout << "--------------------------------------------------------------------------" << endl;
    cout << endl;
    cout<<"Done in "<< total <<" s."<<endl;
    cout << " ==================================================" << endl;
    cout << " == General MAP: " << mapr_total/100 << "    General P@10: " << p10_total/100 << " ==" << endl;
    cout << " ==================================================" << endl;

    results_file << " ==================================================" << endl;
    results_file << " == General MAP: " << mapr_total/100 << "    General P@10: " << p10_total/100 << " ==" << endl;
    results_file << " ==================================================" << endl;
    results_file << endl;
    results_file << endl;
    results_file.close();
    return 0;
}
