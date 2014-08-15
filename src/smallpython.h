/* [PyObject] */
#include <map>
#include <string>
using std::map;
using std::string;

#define PyObject_HEAD \
	int refCount; \
	struct tagPyTypeObject *type

#define PyObject_HEAD_INIT(typePtr) \
	0, typePtr

typedef struct tagPyObject
{
	PyObject_HEAD;
} PyObject;

/* [PyTypeObject] */
typedef void (*PrintFun) (PyObject* object);
typedef PyObject* (*AddFun) (PyObject *left, PyObject* right);
typedef long (*HashFun) (PyObject* object);

typedef struct tagPyTypeObject
{
	PyObject_HEAD;
	char* name;
	PrintFun print;
	AddFun add;
	HashFun hash;
} PyTypeObject;

PyTypeObject PyType_Type = 
{
	PyObject_HEAD_INIT(&PyType_Type),
	"int",
	0,
	0,
	0
};

static void int_print(PyObject *object);
static PyObject *int_add(PyObject *left, PyObject *right);
static long int_hash(PyObject *object);
static void string_print(PyObject *object);
static PyObject *string_add(PyObject *object, PyObject *right);
static long string_hash(PyObject *object);
static void dict_print(PyObject *object);
PyObject *PyDict_Create();

PyTypeObject PyInt_Type = 
{
	PyObject_HEAD_INIT(&PyType_Type),
	"int",
	int_print,
	int_add,
	int_hash
};

PyTypeObject PyString_Type =
{
	PyObject_HEAD_INIT(&PyType_Type),
	"str",
	string_print,
	string_add,
	string_hash
};

PyTypeObject PyDict_Type =
{
	PyObject_HEAD_INIT(&PyType_Type),
	"dict", 
	dict_print,
	0,
	0
};

/* [PyIntObject] */
typedef struct tagPyIntObject
{
	PyObject_HEAD;
	int value;
} PyIntObject;

/* [PyStringObject] */
typedef struct tagPyStrObject
{
	PyObject_HEAD;
	int length;
	long hashValue;
	char value[50];
}PyStringObject;

/* [PyDictObject] */
typedef struct tagPyDictObject
{
	PyObject_HEAD;
	map<long, PyObject*> dict;
} PyDictObject;

/* [Excute Command] */
void ExcuteCommand(string &command);
void ExcuteAdd(string& target, string& source);
PyObject *GetObjectBySymbol(const string &symbol);
void ExcutePrint(const string& symbol);

PyObject *m_LocalEnvironment = (PyObject*)PyDict_Create();
