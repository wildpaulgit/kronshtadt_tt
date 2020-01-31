/*
Тестовое задание.

Складывает и вычтитате числа произвольной разрядности

Приложение выводит в консоль резудьтаты тестов и, если они успешны, 
запрашивает две строки,
в которые д.б. введены складываемые числа в формате 
[spaces][+|-][0..9]+[spaces]
и выводит результат в консоль.

*/

#include <cstdint>
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <ctime>


class BigNumber
{
    std::vector<int8_t> digits;
    int8_t sign = 0;

public:
    bool IsNAN() const { return !digits.size(); }
    size_t Len() const { return digits.size(); }
    bool IsZero() const { return sign == 0; }
    int8_t Sign() const { return sign; }

    BigNumber operator+(const BigNumber &src)
    {
        return BigNumber::Add(*this, src);
    }

    enum class States
    {
        TSpaces,
        Digits,
        LSpaces
    };

    static BigNumber Parse(const std::string &str)
    {
        States state = States::TSpaces;
        int8_t sign = 0;
        BigNumber res;

        for (auto rit = str.rbegin(); rit != str.rend(); rit++)
        {
            int8_t ch = *rit;

            switch (state)
            {
            case States::TSpaces: // хвостовые пробелы
                if (isspace(ch))
                    continue;
                if (ch >= '0' && ch <= '9')
                {
                    res.digits.push_back(ch - '0');
                    state = States::Digits;
                    continue;
                }
                return BigNumber(); // кривые сиволы справа

            case States::Digits: // тело - цифры или знак
                if (ch >= '0' && ch <= '9')
                {
                    res.digits.push_back(ch - '0');
                    continue;
                }
                else if (ch == '-' || ch == '+')
                {
                    if (res.digits.size() == 0)
                        return BigNumber(); // знак без цифр
                    sign = ch == '+' ? 1 : -1;
                    state = States::LSpaces;
                    continue;
                }
                if (isspace(ch))
                {
                    state = States::LSpaces;
                    continue;
                }
                return BigNumber(); // кривые символы справа от цифр

            case States::LSpaces:
                if (isspace(ch))
                    continue;
                return BigNumber(); //  кривые сиволы после пробелов слева
            }
        }
        if (!sign)
            sign = 1; // явно не указано - положительное
        res.sign = sign;
        res.CorrectForm(); // ведущие 0 и знак 0

        return res;
    }

    std::string ToString()
    {
        if (IsNAN())
            return "NAN";
        std::string ret = std::string(Len() + (sign < 0 ? 1 : 0), '0');
        int i = sign < 0 ? 1 : 0;
        
        for (auto rit = digits.rbegin(); rit != digits.rend(); rit++)
            ret[i++] = '0' + *rit;
        if (sign < 0)
            ret[0] = '-';
        return ret;
    }

    private:
        // убирает ведущие 0
        // убирает знак у 0
        void CorrectForm()
        {
            int i;
            for (i = digits.size() - 1; i > 0; i--)
                if (digits[i])
                    break;
            if (i + 1 != digits.size())
                digits.resize(i + 1);
            if (digits.size() == 1 && !digits[0])
                sign = 0;
        }
        static BigNumber Add(const BigNumber &sn1, const BigNumber &sn2)
        {
            if (sn1.IsNAN() || sn2.IsNAN())
                return BigNumber();

            if (sn1.IsZero() || sn2.IsZero())
                return sn1.IsZero() ? sn2 : sn1;

            // в случае вычитания n1 - уменшаемое
            // при сложении не важно
            const BigNumber &n1 = sn1.sign > 0 ? sn1 : sn2;
            const BigNumber &n2 = sn1.sign > 0 ? sn2 : sn1;

            bool sub = n1.sign != n2.sign;
            int numdig = std::max(n1.Len(), n2.Len());

            BigNumber res;
            res.digits.resize(numdig, 0);

            int8_t over = 0;

            for (int i = 0; i < numdig; i++)
            {
                int8_t d1 = i < n1.Len() ? n1.digits[i] : 0;
                int8_t d2 = i < n2.Len() ? n2.digits[i] : 0;
                if (sub)
                    d2 = 9 - d2; // дополнение до 9

                int8_t s = d1 + d2 + over;
                over = s >= 10 ? 1 : 0;
                res.digits[i] = s % 10;
            }

            if (sub)
            {
                res.sign = over ? 1 : -1; // смена знака по признаку переноса старшего разряда
                if (over)                 // +1 к младшему
                {
                    for (auto &r : res.digits)
                    {
                        if (r == 9)
                        {
                            r = 0;
                            over = 1;
                            continue;
                        }
                        r++;
                        over = 0;
                        break;
                    }
                }
                else
                    // обратное дополнение до 9
                    for (auto &r : res.digits)
                        r = 9 - r;
            }
            else
                res.sign = n1.sign;

            // при переполнении добавляем 1 в старшем разряде
            if (over)
                res.digits.push_back(1);
            else if (sub)
                res.CorrectForm();

            return res;
        }
    };

    int main()
    {
        std::srand(unsigned(std::time(0)));

        std::string strings[] =
            {
                "",                "NAN",
                " ",                "NAN",
                "+",                "NAN",
                "-",                "NAN",
                "++1",              "NAN",
                "1+",               "NAN",
                "+ 1",              "NAN",
                "0 0",              "NAN",
                "0+1",              "NAN",
                "=",                "NAN",
                "0",                "0",
                "00",               "0",
                "-0",               "0",
                "+0",               "0",
                "+1",               "1",
                "+0001",            "1",
                "-1",              "-1",
                "-00001",          "-1",
                "-12345678901234567890123456789012345678901234567890",
                "-12345678901234567890123456789012345678901234567890",
                "12345678901234567890123456789012345678901234567890",
                "12345678901234567890123456789012345678901234567890",
                "+12345678901234567890123456789012345678901234567890",
                "12345678901234567890123456789012345678901234567890",

            };

        int failcnt = 0;

        for (int i = 0; i < sizeof(strings) / sizeof(strings[0]); i += 2)
        {
            BigNumber b = BigNumber::Parse(strings[i]);
            std::cout << strings[i] 
            << " | " 
            << strings[i+1] 
            << " = " 
            <<  b.ToString() 
            << "\n";

            if(b.ToString() != strings[i+1])
            {
                std::cout << "FAIL\n";
                failcnt ++;
            }
        }

        std::vector<int> test = {0, 0,
                      0, 1,
                      1, 0,
                      1, 1,
                      0, -1,
                      -1, 0,
                      -1, -1,
                      -1, 1,
                      1, -1,

                      99, 1,
                      1, 99,
                      1, -99,
                      -99, 1,
                      -1, 99,
                      99, -1,
                      -99, -1,
                      -1, -99,
                       1111,  9999,
                      -1111, -9999};

        for( int i=0; i<10; i++)
        {
          test.push_back( std::rand() - RAND_MAX/2);
          test.push_back( std::rand() - RAND_MAX/2);
        }

        for (int i = 0; i < test.size(); i += 2)
        {
            int a = test[i];
            int b = test[i + 1];
            int ab = a + b;

            std::cout << "*******\n"
                      << a << " + " << b << " = " << ab << "\n";

            auto sa = std::to_string(a);
            auto sb = std::to_string(b);
            auto sab = std::to_string(ab);

            BigNumber ba = BigNumber::Parse(sa);
            BigNumber bb = BigNumber::Parse(sb);

            BigNumber bab = ba + bb;

            std::cout << ba.ToString() << " + " << bb.ToString() << " = " << bab.ToString() << "\n";

            std::string sbab = bab.ToString();
            if (sbab != sab)
            {
                std::cout << "FAIL!\n";
                failcnt++;
            }
        }

        if( !BigNumber::Parse("0").IsZero() )
        {
           std::cout << "ZERO TEST FAIL!\n";
           failcnt++;    
        } 

        if( BigNumber::Parse("0").Sign() )
        {
           std::cout << "ZERO SIGN TEST FAIL!\n";
           failcnt++;    
        } 
        
        if( BigNumber::Parse("-1").IsZero() )
        {
           std::cout << "NON ZERO TEST FAIL!\n";
           failcnt++;    
        } 

        if( BigNumber::Parse("-1").Sign() >= 0 )
        {
           std::cout << "NEGATIVE SIGN TEST FAIL!\n";
           failcnt++;    
        }

        if( BigNumber::Parse("1").Sign() <= 0 )
        {
           std::cout << "POSITIVE SIGN TEST FAIL!\n";
           failcnt++;    
        }         

        if(failcnt)
        {
            std::cout << "\nTEST FAIL\n";
            return 1;
        }else{
            std::cout << "\nALL TESTS PASSED OK\n";
        }

        std::vector<BigNumber> numbers(2, BigNumber());

        int i = 0;
        for (auto &num : numbers)
        {
            std::cout << ++i << " Enter number and press enter: ";
            std::string numstr;
            getline(std::cin, numstr);
            num = BigNumber::Parse(numstr);
            if (num.IsNAN())
            {
                std::cerr << "Wrong string: " << numstr;
                return 1;
            }
        }
        std::cout << (numbers[0] + numbers[1]).ToString();
    }