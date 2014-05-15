typedef long Value;

int isNull(Value value);
int isBoolean(Value value);
int isTrue(Value value);
int isFalse(Value value);
int isObject(Value value);
int isFixnum(Value value);
int isString(Value value);
int isDouble(Value value);

Value boxNull();
Value boxObject(void* object);
Value boxBoolean(int value);
Value boxFixnum(long fixnum);
Value boxDouble(double value);
Value boxString(size_t length, char* string);

void*  unboxObject(Value value);
int    unboxBoolean(Value value);
long   unboxFixnum(Value value);
double unboxDouble(Value value);
char*  unboxString(Value value);
size_t unboxStringLength(Value value);
