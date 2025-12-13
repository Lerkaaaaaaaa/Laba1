1. 
#include <iostream>
using namespace std;
int main()
{
    setlocale(0, "");
    cout 
        << "int: " << sizeof(int) << " байта\n"
        << "short int: " << sizeof(short int) << " байта\n"
        << "long int: " << sizeof(long int) << " байта\n"
        << "long long int: " << sizeof(long long int) << " байта\n"
        << "float: " << sizeof(float) << " байта\n"
        << "double: " << sizeof(double) << " байта\n"
        << "long double: " << sizeof(long double) << " байта\n"
        << "char: " << sizeof(char) << " байта\n"
        << "bool: " << sizeof(bool) << " байта\n";
}

2.
#include <iostream>
using namespace std;
int main()
{
   int value = -127;
   int order = sizeof(int) * 8;
   unsigned int mask = 1 << (order - 1);
   
   for (int i = 0; i < order; i++) {
       if (value & (mask >> i)) {
           cout << "1";
       }
       else {
           cout << "0";
       }
   }
}


3.
#include <iostream>
using namespace std;

void showBinary(float number) {
    float num = number;
    unsigned int *p = (unsigned int*)&num;
    unsigned int bits = *p;
    cout << "Двоичное представление числа " << number << ":\n";
    cout << "Знак: " << ((bits & 0x80000000) >> 31) << "\n";
    cout << "Порядок: ";
    for(int i = 7; i >= 0; i--) {
        cout << ((bits & (0x7F << 23)) >> (23 + i));
    }
    cout << "\n";
    cout << "Мантисса: ";
    for(int i = 22; i >= 0; i--) {
        cout << ((bits & (0x1 << i)) >> i);
    }
    cout << "\n";
    cout << "\nПолное представление:\n";
    cout << "Знак Порядок Мантисса\n";
    cout << ((bits & 0x80000000) >> 31) << "  ";

    for(int i = 7; i >= 0; i--) {
        cout << ((bits & (0x7F << 23)) >> (23 + i));
    }
    cout << "  ";
    for(int i = 22; i >= 0; i--) {
        cout << ((bits & (0x1 << i)) >> i);
    }
    cout << "\n";
}

int main() {
    float number = 3.14f;  
    showBinary(number);
    return 0;
}


4.
#include <iostream>
using namespace std;

unsigned int changeBit(unsigned int number, int position, bool value) {
    unsigned int mask = 1 << position;
    if (value) {
        return number | mask;
    } else {
        return number & ~mask;
    }
}

int main() {
    setlocale(0, "");

    unsigned int number;
    int position;
    bool value;

    cout << "Введите число: ";
    cin >> number;

    cout << "Введите позицию бита (0-31): ";
    cin >> position;

    cout << "Введите значение (0 или 1): ";
    cin >> value;

    unsigned int result = changeBit(number, position, value);

    cout << "Исходное число: " << number << endl;
    cout << "Результат: " << result << endl;

    return 0;
}


5.
#include <cstdint> 
#include <iostream> 
#include <windows.h> // для SetConsoleOutputCP и system("pause") 
 
using namespace std; 
 
// Доступ к числу как float и uint32_t 
union FloatBits 
{ 
  float f; 
  uint32_t u; 
}; 
 
// Битовое сложение x + 1.0f 
float addOneBitwise(float x) 
{ 
  FloatBits a; 
  a.f = x; 
 
  // Разбираем на части 
  uint32_t sign = (a.u >> 31) & 1u; 
  uint32_t exp = (a.u >> 23) & 0xFFu; 
  uint32_t frac = a.u & 0x7FFFFFu; 
 
  // NaN или Inf 
  if (exp == 0xFFu) 
    return x; 
 
  // Только для положительных 
  if (sign == 1u) 
    return x + 1.0f; 
 
  // Ноль 
  if (exp == 0 && frac == 0) 
    return 1.0f; 
 
  uint32_t mantX; 
 
  if (exp == 0) 
  { 
    // Денормализованное число: скрытой единицы нет. 
    exp = 1;      // считаем порядок равным 1 
    mantX = frac; // мантисса как есть 
  } 
  else 
  { 
    // Нормализованное: добавляем 1 
    mantX = (1u << 23) | frac; 
  } 
 
  // 1.0f 
  const uint32_t expOne = 127; 
  const uint32_t mantOne = (1u << 23); 
 
  // Сортировка по порядку 
  uint32_t bigExp, smallExp; 
  uint32_t mantBig, mantSmall; 
 
  if (exp >= expOne) 
  { 
    bigExp = exp; 
    mantBig = mantX; 
    smallExp = expOne; 
    mantSmall = mantOne; 
  } 
  else 
  { 
    bigExp = expOne; 
    mantBig = mantOne; 
    smallExp = exp; 
    mantSmall = mantX; 
  } 
 
  // Разница порядков 
  uint32_t shift = bigExp - smallExp; 
 
  // Если разница велика, результат очевиден 
  if (shift >= 24) 
  { 
    if (exp >= expOne) 
      return x; // x намного больше 1.0 
    else 
      return 1.0f; // 1.0 намного больше x 
  } 
 
  // Выравнивание 
  mantSmall >>= shift; 
 
  // Сложение мантисс 
  uint32_t mantSum = mantBig + mantSmall; 
  uint32_t resExp = bigExp; 
 
  // Переполнение мантиссы 
  if (mantSum & (1u << 24)) 
  { 
    mantSum >>= 1; 
    resExp++; 
 
    // Переполнение порядка (Inf) 
    if (resExp >= 255) 
    { 
      FloatBits inf; 
      inf.u = (0u << 31) | (0xFFu << 23); // знак 0, порядок все 1, мантисса 0 
      return inf.f; 
    } 
  } 
 
  // Убираем скрытую единицу 
  uint32_t resFrac = mantSum & 0x7FFFFFu; 
 
  // Сборка результата 
  FloatBits res; 
  res.u = (0u << 31) | (resExp << 23) | resFrac; 
 
  return res.f; 
} 
 
int main() 
{ 
  // UTF-8 
  SetConsoleOutputCP(65001); 
 
  float x; 
  cout << "Введите число (float): "; 
  cin >> x; 
 
  float y = addOneBitwise(x); 
 
  cout << "Результат битового сложения с 1.0: " << y << endl; 
 
  // Проверка 
  cout << "Проверка обычным сложением      : " << (x + 1.0f) << endl; 
 
  system("pause"); 
  return 0; 
}


28.ИДЗ

```cpp
#include <iostream>

struct FloatParts {
    bool sign;
    int exponent;
    int mantissa;
};

FloatParts addFloats(const FloatParts& num1, const FloatParts& num2) {
    FloatParts result;
    
    result.sign = false;
    
    int exp1 = num1.exponent;
    int exp2 = num2.exponent;
    int mant1 = num1.mantissa;
    int mant2 = num2.mantissa;
    
    if (exp1 > exp2) {
        mant2 >>= (exp1 - exp2);
        result.exponent = exp1;
    } else if (exp2 > exp1) {
        mant1 >>= (exp2 - exp1);
        result.exponent = exp2;
    } else {
        result.exponent = exp1;
    }
    
    result.mantissa = mant1 + mant2;
    
    const int MAX_MANTISSA = 255;
    
    if (result.mantissa > MAX_MANTISSA) {
        result.mantissa >>= 1;
        result.exponent++;
    }
    
    return result;
}

int main() {
    FloatParts num1 = {false, 0, 128};
    FloatParts num2 = {false, 0, 128};
    
    FloatParts result = addFloats(num1, num2);
    
    std::cout << "Результат сложения:" << std::endl;
    std::cout << "Знак: " << (result.sign ? "-" : "+") << std::endl;
    std::cout << "Порядок (экспонента): " << result.exponent << std::endl;
    std::cout << "Мантисса: " << result.mantissa << std::endl;
    
    std::cout << "Приблизительное значение: " << (result.mantissa / 128.0) << std::endl;
    
    return 0;
}

