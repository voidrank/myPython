
#include "string.h"
#include "smallpython.h"
#include "stdio.h"
#include "stdlib.h"
#include <string>
#include <iostream>

using std::string;
using std::cout;
using std::cin;
using std::endl;

/* [PyIntObject] */
PyObject* PyInt_Create(int value)
{
	PyIntObject* object = new PyIntObject;
	object->refCount = 1;
	object->type = &PyInt_Type;
	object->value = value;
	return (PyObject*) object;
}


static void int_print(PyObject *object)
{
	PyIntObject *intObject = (PyIntObject*)object;
	printf("%d\n", intObject->value);
}

static PyObject *int_add(PyObject *left, PyObject *right){
	PyIntObject *leftInt = (PyIntObject*) left;
	PyIntObject *rightInt = (PyIntObject*) right;
	PyIntObject *result = (PyIntObject*) PyInt_Create(0);
	if (result == NULL)
	{
		printf("wo have no enough memory!!");
		exit(1);
	}
	else
	{
		result->value = leftInt->value + rightInt->value;
	}
	return (PyObject*) result;
}

static long int_hash(PyObject* object)
{
	return (long) ((PyIntObject*)object)->value;
}

/* [PyStringObject] */
PyObject *PyStr_Create(const char *value)
{
	PyStringObject* object = new PyStringObject;
	object->refCount = 1;
	object->type = &PyString_Type;
	object->length = (value==NULL) ? 0 : strlen(value);
	object->hashValue = -1;
	memset(object->value, 0, 50);
	if (value != NULL)
	{
		strcpy(object->value, value);
	}
	return (PyObject*) object;
}

void string_print(PyObject *object)
{
	PyStringObject *strObject = (PyStringObject*) object;
	printf("%s\n", strObject->value);
}

static long string_hash(PyObject *object)
{
	PyStringObject *strObject = (PyStringObject*) object;
	register int len;
	register unsigned char *p;
	register long x;

	if (strObject->hashValue != -1)
		return strObject->hashValue;
	len = strObject->length;
	p = (unsigned char *)strObject->value;
	x = *p << 7;
	while (--len >= 0)
		x = (1000003*x) ^ *p++;
	x ^= strObject->length;
	if (x == -1)
		x = -2;
	strObject->hashValue = x;
	return x;
}

static PyObject *string_add(PyObject *left, PyObject *right)
{
	PyStringObject *leftStr = (PyStringObject*) left;
	PyStringObject *rightStr = (PyStringObject*) right;
	PyStringObject *result = (PyStringObject*) PyStr_Create(NULL);
	if (result == NULL)
	{
		printf("We have no enough memory!!");
		exit(1);
	}
	else
	{
		strcpy(result->value, leftStr->value);
		strcat(result->value, rightStr->value);
	}
	return (PyObject*) result;
}

/* [PyDictObject] */
PyObject *PyDict_Create()
{
	PyDictObject *object = new PyDictObject;
	object->refCount = 1;
	object->type = &PyDict_Type;

	return (PyObject*) object;
}

PyObject *PyDict_GetItem(PyObject *target, PyObject *key)
{
	long keyHashValue = (key->type)->hash(key);
	map<long, PyObject*>& dict = ((PyDictObject*)target)->dict;
	map<long, PyObject*>::iterator it = dict.find(keyHashValue);
	map<long, PyObject*>::iterator end = dict.end();
	if (it == end)
	{
		return NULL;
	}
	return it->second;
}

int PyDict_SetItem(PyObject *target, PyObject *key, PyObject *value)
{
	long keyHashValue = (key->type)->hash(key);
	PyDictObject *dictObject = (PyDictObject*) target;
	(dictObject->dict)[keyHashValue] = value;
	//cout << dictObject->dict.size() << endl;
	//rPyObject *t = dictObject->dict.find(keyHashValue)->second;
	t->type->print(t);
	return 0;
}

static void dict_print(PyObject* object)
{
	PyDictObject *dictObject = (PyDictObject*)object;
	printf("{");
	map<long, PyObject*>::iterator it = (dictObject->dict).begin();
	map<long, PyObject*>::iterator end = (dictObject->dict).end();
	for ( ; it != end; ++it)
	{
		//print key
		printf("%ld : ", it->first);
		//print value
		PyObject* value = it->second;
		(value->type)->print(value);
		printf(", ");
	}
	puts("}");
}

/* ExcuteCommand */

void ExcuteCommand(string& command)
{
	string::size_type pos = 0;
	if ((pos = command.find("print ")) != string::npos) 
	/*	Maximum value for size_t
		npos is a static member constant value with the greatest possible value for an element of type size_t.
		This value, when used as the value for a len (or sublen) parameter in string's member functions, means "until the end of the string".
		As a return value, it is usually used to indicate no matches.
		This constant is defined with a value of -1, which because size_t is an unsigned integral type, it is the largest possible representable value for this type.
	*/
	{
		ExcutePrint(command.substr(6));
	}
	else if ((pos = command.find(" = ")) != string::npos)
	{
		string target = command.substr(0, pos);
		string source = command.substr(pos+3);
		ExcuteAdd(target, source);
	}
}

bool IsSourceAllDigit(string &source)
{
	bool is_source_all_dight = 1;
	string::iterator it = source.begin();
	string::iterator end = source.end();
	for ( ; it != end; ++it )
		if (*it > '9'|| *it < '0')
		{
			is_source_all_dight = 0;
			break;
		}
	return is_source_all_dight;
}

void ExcuteAdd(string &target, string &source)
{
	string::size_type pos;
	PyObject *strValue = NULL;
	PyObject *resultValue = NULL;
	if (IsSourceAllDigit(source))
	{
		PyObject* intValue = PyInt_Create(atoi(source.c_str()));
		PyObject* key = PyStr_Create(target.c_str());
		PyDict_SetItem(m_LocalEnvironment, key, intValue);
	}
	else if (source.find("\"") != string::npos)
	{
		strValue = PyStr_Create(source.substr(1, source.size() - 2).c_str());
		PyObject *key = PyStr_Create(target.c_str());
		PyDict_SetItem(m_LocalEnvironment, key, strValue);
	}
	else if ((pos = source.find("+")) != string::npos)
	{
		PyObject *leftObject = GetObjectBySymbol(source.substr(0, pos));
		PyObject *rightObject = GetObjectBySymbol(source.substr(pos+1));
		if (leftObject != NULL && rightObject != NULL && leftObject->type == rightObject->type)
		{
			resultValue = (leftObject->type)->add(leftObject, rightObject);
			PyObject *key = PyStr_Create(target.c_str());
			PyDict_SetItem(m_LocalEnvironment, key, resultValue);
		}
	}
}

PyObject *GetObjectBySymbol(const string &symbol)
{
	PyObject *key = PyStr_Create(symbol.c_str());
	PyObject *value = PyDict_GetItem(m_LocalEnvironment, key);
	if (value == NULL)
	{
		cout << "[Error] : " << symbol << " is not defined!!" << endl;
		return NULL;
	}
	return value;
}

void ExcutePrint(const string& symbol)
{
	PyObject *object = GetObjectBySymbol(symbol);
	if (object != NULL)
	{
		PyTypeObject* type = object->type;
		type->print(object);
	}
}

/* [interactive] */

const char *info = "***********  Python Research  ***********\n";
const char *prompt = ">>> ";

string m_Command;

void Excute()
{
	cout << info;
	cout << prompt;
	while (getline(cin, m_Command))
	{
		if (m_Command.size() == 0)
		{
			cout << prompt;
			continue;
		}
		else if (m_Command == "exit")
		{
			return;
		}
		else
		{
			ExcuteCommand(m_Command);
		}
		cout << prompt;
	}
}

int main(){
	Excute();
	return 0;
}