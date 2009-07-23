/*=============================================================================
    Copyright (c) 2002-2003 Joel de Guzman
    http://spirit.sourceforge.net/

    Use, modification and distribution is subject to the Boost Software
    License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
    http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/
///////////////////////////////////////////////////////////////////////////////
//
//  This sample demontrates a parser for a comma separated list of numbers
//  This is discussed in the "Quick Start" chapter in the Spirit User's Guide.
//
//  [ JDG 5/10/2002 ]
//
///////////////////////////////////////////////////////////////////////////////
#include <boost/spirit/core.hpp>
#include <boost/spirit/actor/push_back_actor.hpp>
#include <boost/spirit/dynamic/stored_rule.hpp>
#include <boost/any.hpp>
#include <iostream>
#include <vector>
#include <string>
#include <functional>
///////////////////////////////////////push_back(const T&)////////////////////////////////////////
using namespace std;
using namespace boost::spirit;
using namespace boost;

///////////////////////////////////////////////////////////////////////////////
//
//  Our comma separated list parser
//
///////////////////////////////////////////////////////////////////////////////
#define BOOST_SPIRIT_DEBUG 


void action_key(const char* start,  const char* end)
{
  cout << std::string(start, end)<<endl;
}

typedef rule<phrase_scanner_t> _r1, _r;
typedef stored_rule<phrase_scanner_t> _sr;
typedef rule<> _r2;


// _r2 karg 	=  *(space_p)>> (+(~ch_p(',') & ~ch_p(')')  & ~ch_p('(') &~space_p))[&action_key]>>*(space_p);
// _r2 args 	= ch_p('(')>> karg >> *(ch_p(',') >> karg) >>ch_p(')');
// _r2 keyword 	=  ch_p('%')>> (str_p("key") | str_p("ciccio") | str_p("pluto") | str_p("paperino") )[&action_key];
// _r2 keyword_args =  keyword >> *(space_p) >> *(args)[&action_key];
// _r2 line 	= *(~ch_p('%')) >>keyword_args >> *(+(space_p) >> keyword_args) >> *(space_p) ;
// 

// template <typename Class_ref, typename Func_ptr>
// class caller
// {
// public:
//   caller(Class_ref class_ref, Func_ptr func_ptr), _class_ref(class_ref), _func_ptr(func_ptr){}
//   template <typename arg1, typename arg2>
//   void operator() (arg1 start,  arg1 end) const
//   {
//     _class_ref->*_func_ptr(start,  end);
//   }
// private:
//   Class_ref _class_ref;
//   Func_ptr func_ptr;
// };

class pippo
{
public:
  void pluto (const char* a, const char* b)
  {
    cout << a << " , " << b<< endl;
  }
};

typedef pippo* Class_ref;
typedef  void (pippo::*Func_ptr)(const char*, const char*);
typedef const char* arg1;
typedef const char* arg2;


template <typename Class_ptr, typename Func_ptr>
class caller
{
private:
  Class_ptr _class_ref;
  Func_ptr _func_ptr;
public:
  caller(Class_ptr class_ref, Func_ptr func_ptr): _class_ref(class_ref), _func_ptr(func_ptr){}

  void operator() () const
  {
    (_class_ref->*_func_ptr)();
  }

  template <typename arg1>
  void operator() (arg1 a1) const
  {
    (_class_ref->*_func_ptr)(a1);
  }
  
  template <typename arg1, typename arg2>
  void operator() (arg1 a1,  arg2 a2) const
  {
    (_class_ref->*_func_ptr)(a1,  a2);
  }

  template <typename arg1, typename arg2, typename arg3>
  void operator() (arg1 a1,  arg2 a2, arg3 a3) const
  {
    (_class_ref->*_func_ptr)(a1,  a2, a3);
  }
};

template <typename Class_ptr, typename Func_ptr>
caller<Class_ptr, Func_ptr> caller_make(Class_ptr c, Func_ptr f) { return caller<Class_ptr, Func_ptr> (c, f);}

void f ()
{
  pippo p;

  caller_make(&p, &pippo::pluto)("ciao", "pippo");
}


bool equal (const any& arg1, const any& arg2)
{
  const type_info& ti (arg1.type());
  const type_info& ti2 (arg2.type());
  if (ti == ti2)
  {
    if (ti == typeid(int))
      return any_cast<int>(arg1) ==  any_cast<int>(arg2);
    if (ti == typeid(string))
      return any_cast<string>(arg1) ==  any_cast<string>(arg2);
  }
  return false;
}

class myexception: public std::exception
{
public:
  myexception(const char* w):_what(w){};
  myexception(){};
  virtual const char* what() const throw()
  {
    return _what.c_str();
  }
  ~myexception()throw(){};
private:
  string _what;
};


bool nequal (const any& arg1, const any& arg2)
{
  return ! equal(arg1, arg2);
}

bool greater_(const any& arg1, const any& arg2)
{
  const type_info& ti (arg1.type());
  const type_info& ti2 (arg2.type());
  if (ti == ti2)
  {
    if (ti == typeid(int))
      return any_cast<int>(arg1) >  any_cast<int>(arg2);
    if (ti == typeid(string))
      return any_cast<string>(arg1) >  any_cast<string>(arg2);
  }
  throw  myexception("uncompatible types");
}

bool less_ (const any& arg1, const any& arg2)
{
  return any_cast<int>(arg1) < any_cast<int>(arg2);
  //return (int)arg1 < (int)arg2;
}

bool and_ (bool arg1, bool arg2)
{
  return (arg1 && arg2);
}

bool or_ (bool arg1, bool arg2)
{
  return (arg1 || arg2);
}

template <typename T>
T pop_back(vector<T> & v)
{
    T t = v.back();
    v.pop_back();
    return t;
}
template <typename T>
void push_back(vector<T> & v, T t)
{
    v.push_back(t);
}


class test
{

/*    class caller
    {
	test* _pTest;
    public:
	caller(test* pTest) :_pTest(pTest){};
	void operator() (const char* start,  const char* end) const
	{
	  _pTest->action_key2(start,  end);
	}
    };*/
//     _r2 karg;
//     _r2 args;
//     _r2 keyword;
//     _r2 keyword_args;
//     _r2 line;
  typedef bool (*binary_func)(const any&, const any&);
  typedef bool (*logic_func)(bool , bool);

  _r operand;
  _r keyword;
  _r string_;
  _r term;
  _r logic_operand;
  _r atomic_expression, expression;
  _r group;
  _r group_or_expr;
  _r complex_expression, final_expression, logic_espression_, logic_espression_p, atomic_logic_espression;
  _r func_arg;
  _r func_name;
  _r function;
  _r integer;
  any value;
  vector<any> terms;
  vector<binary_func> operands;
  vector<logic_func> logic_operands;
  any term1, term2;
  logic_func logic_op;
  
  bool first_expression, second_expression, logic_expression;

  static void action_key(const char* start,  const char* end)
  {
    cout << std::string(start, end)<<endl;
  }
  void on_key(const char* start,  const char* end)
  {
    cout << this << " on_key : " <<std::string(start, end)<<endl;
    value = std::string(start, end);
  }
  void on_expression(const char* start,  const char* end)
  {
    binary_func operand = pop_back(operands);
    push_back(espr_results, operand(pop_back(terms), pop_back(terms)));
    
  }
  void on_string(const char* start,  const char* end)
  {
    value = string(start, end);
    //cout << this << " on_string: " <<std::string(start, end)<<endl;
  }
  void on_integer(int val)
  {
    value = val;
    //cout << this << " on_integer: " <<val<<endl;
  }
  void on_op(const char* start,  const char* end)
  {
    string operand = string (start, end);
    binary_func op;
    if (operand == "==")
      op = equal;
    if (operand == "!=")
      op = nequal;
    if (operand == ">")
      op = greater_;
    if (operand == "<")
      op = less_;
    push_back(operands, op);
    //cout << this << " on_op: " <<std::string(start, end)<<endl;
  }
  void on_term(const char* start,  const char* end)
  {
    push_back(terms, value);
    //term1 = value;
    //cout << this << " on_term1: " <<std::string(start, end)<<endl;
  }

  void on_logic_op(const char* start,const char* end)
  {
    string operand = string (start, end);
    if (operand == "||")
      logic_op = or_;
    if (operand == "&&")
      logic_op = and_;

    push_back (logic_operands, logic_op);
  }

  void on_complex_espression (const char* start,  const char* end)
  {
    bool v1 = pop_back(espr_results);
    while (!espr_results.empty())
    {
	bool v2 = pop_back(espr_results);
	logic_func op = pop_back(logic_operands);
	push_back(espr_results, op(v1, v2));
	v1 = pop_back(espr_results);
    }
    push_back(espr_results, v1);
    //cout << this << " on_complex_espression " <<first_expression <<" "<< string (start, end)<< " "<< expr_value<<endl;
  }
  void on_logic_expression(const char* start,  const char* end)
  {
    logic_expression = logic_op(first_expression, second_expression);
    cout << this << " logic_expression " <<logic_expression <<" "<< string (start, end)<<endl;
  }

  typedef vector<const char*> v_t;
  _sr res; 
  typedef uint_parser<unsigned, 10, 1, 9> uint_p;
  v_t v;
public:
  test()
  {
    v.push_back("key");
    v.push_back("ciccio");
    v.push_back("pluto");
    v.push_back("paperino");
    v_t::iterator end = v.end();
    v_t::iterator i = v.begin();
    cout << " start "<< endl;
    bool first =true;
    for (;  i != end; i++)
    {
      cout << " iter :";
      if (first)
      {
	res = str_p(*i);
      }
      else
      {
	res = res.copy() | str_p(*i);
      }
      first = false;
      cout <<*i<< endl;
    }
    cout << " end "<< endl;
    
    //caller(this)("cuai", "siuso");
//     karg 		=  *(space_p)>> (+(~ch_p(',') & ~ch_p(')')  & ~ch_p('(') &~space_p))[c]>>*(space_p);
//     args 		=  ch_p('(')>> karg >> *(ch_p(',') >> karg) >>ch_p(')');
//     keyword 		=  ch_p('%')>> (str_p("key") | str_p("ciccio") | str_p("pluto") | str_p("paperino") )[c];
//     keyword_args 	=  keyword >> *(space_p) >> *(args)[c];
//     line 		= *(~ch_p('%')) >>keyword_args >> *(+(space_p) >> keyword_args) >> *(space_p) ;

    operand 	= (str_p("==") | str_p("!=") | str_p(">") | str_p("<"))[caller_make(this, &test::on_op)];
    keyword 	= lexeme_d[ch_p('%')>> (res )[caller_make(this, &test::on_key)]];
    string_ 	= lexeme_d[ch_p('\'') >> (*(~ch_p('\''))) [caller_make(this, &test::on_string)]>>ch_p('\'')];
    integer	= int_p[caller_make(this, &test::on_integer)];
    func_arg 	= keyword | string_;
    func_name	= str_p("int");
    function	= func_name >> ch_p('(') >> func_arg >>ch_p(')');
    term  	= keyword  | string_ | function | integer;
    logic_operand = str_p("&&") | str_p("||");
    atomic_expression 	= (term [caller_make(this, &test::on_term)] >> 
			  operand[caller_make(this, &test::on_op)] >> 
			   term[caller_make(this, &test::on_term)])[caller_make(this, &test::on_expression)];
    complex_expression =  (ch_p('(') >> (atomic_expression|complex_expression) >> 
			  *(logic_operand[caller_make(this, &test::on_logic_op)]>> 
			  (atomic_expression|complex_expression))>>ch_p(')'))[caller_make(this, &test::on_complex_espression)];
    
  }

  _r& get_rule(){return complex_expression;}
  vector<bool> espr_results;
};

test t;
bool
parse_numbers(const string& str)
{
    return parse(str.c_str(),

        //  Begin grammar
            //real_p[push_back_a(v)] >> *("---" >> real_p[push_back_a(v)])
	  t.get_rule(), space_p
        
        ).full;
}

////////////////////////////////////////////////////////////////////////////
//
//  Main program
//
////////////////////////////////////////////////////////////////////////////
struct value
{
  value (const char* arg1, bool arg2):val(arg1), expected(arg2){}
  const char* val;
  bool expected;
};

value tests[] = {
value("0 == 9  || ((1 == 2) || (3 == 4))  || ((5==6) ||(7==8))", false),
value("1 == 2 || 3 == 4  || 5==6 ||7==8",false),
value("1 == 2",false),
value("(1 == 2 ) ",false),
value("1 == 2 || 1 == 2",false),
value("(1 == 2 ) || 3 == 4",false),
value("1 == 2  || (3 == 4)",false),
value("(1 == 2 || 3 == 4  || 5==6) ||7==8",false),
value("(1 == 1 ) ",true),
value("1 == 1  ",true),
value("1 == 1 ||  1 == 2",true),
value("1 == 1 &&  2 == 2",true),
value("1 == 1 &&  1 == 2",false),
value("(1 == 1) ||  (1 == 2)",true),
value("(1 == 1 || 3 == 4  ) && (5==6 ||7==8) ",false),
value("(1 == 1 || 3 == 3  ) && (5==6 ||'7'=='7') ",true)
};

void test_case()
{
    int len = sizeof(tests)/sizeof(value);
    cout << len <<endl;
    int counter = 0;
    for (value* it = tests ; counter <len; it ++, counter++)
    {
      string str2;
      str2.append("(");
      str2.append(it->val);
      str2.append(")");
      cout <<str2<< "\n "; 
      if (parse_numbers(str2))
      {
	bool res_expression = pop_back(t.espr_results);
	cout << "result " <<((res_expression )? string("True"):string("False"))<< endl;
	if (res_expression == it->expected)
	  cout <<"OK"<< endl;
	else
	  cout <<"KO"<< endl;

      }
      else
	cout <<"Parsing failed!!"<< endl;

      cout << endl;
    }
}

int
main()
{
    f();
    cout << "/////////////////////////////////////////////////////////\n\n";
    cout << "\t\tA comma separated list parser for Spirit...\n\n";
    cout << "/////////////////////////////////////////////////////////\n\n";

    cout << "Give me a comma separated list of numbers.\n";
    cout << "The numbers will be inserted in a vector of numbers\n";
    cout << "Type [q or Q] to quit\n\n";
    test_case();
    cout << endl;
    string str;
    while (getline(cin, str))
    {
        if (str.empty() || str[0] == 'q' || str[0] == 'Q')
            break;

	string str2;
	str2.append("(");
	str2.append(str);
	str2.append(")");
        if (parse_numbers(str2.c_str()))
        {
            cout << "-------------------------\n";
            cout << "Parsing succeeded\n";
            cout << str << " Parses OK: " << endl;

	    cout << "result " <<((pop_back(t.espr_results))? string("True"):string("False"))<< endl;

            cout << "-------------------------\n";
        }
        else
        {
            cout << "-------------------------\n";
            cout << "Parsing failed\n";
            cout << "-------------------------\n";
        }
    }

    cout << "Bye... :-) \n\n";
    return 0;
}


