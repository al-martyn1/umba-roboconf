# Umba Roboconf

  - [Отказ от ответсвенности](#user-content-отказ-от-ответсвенности)
  - [Umba](#user-content-umba)
  - [Условия использования](#user-content-условия-использования)
  - [Регулярные выражения](#user-content-регулярные-выражения)
  - [Правила](#user-content-правила)
    - [Шаблоны правил](#user-content-шаблоны-правил)
      - [Формат описания правил](#user-content-формат-описания-правил)
        - [Атомы](#user-content-атомы)
        - [Токены T](#user-content-токены-t)
        - [Списки L](#user-content-списки-l)
        - [Значения V](#user-content-значения-v)
        - [S-атомы](#user-content-s-атомы)
        - [Значение или список значений A](#user-content-значение-или-список-значений-a)


# Отказ от ответсвенности

# Umba

# Условия использования


# Регулярные выражения

Регулярные выражения, там, где они используются, используют синтаксис
[Modified ECMAScript regular expression grammar](https://en.cppreference.com/w/cpp/regex/ecmascript),
принятый в C++ как синтаксис регулярных выражений по умолчанию.

# Правила

Все правила программы описываются в виде 
[S-выражений](https://ru.wikipedia.org/wiki/S-%D0%B2%D1%8B%D1%80%D0%B0%D0%B6%D0%B5%D0%BD%D0%B8%D0%B5).
Когда вы пишете или изучаете правила программы, вам может показаться, что вы пишете/читаете программу 
на языке Лисп. И вы будете недалеки от истины.

При первоначальном разборе проверяется только базовая корректность синтаксиса. Полностью корректность 
правил проверяется при извлечении правил при работе подсистем, использующих эти правила.


## Шаблоны правил

Правила извлекаются по шаблонам, которые формализуют конкретные правила. Данные шаблоны представляют 
собой что-то типа DSL (Domain Specific Language - предметно-ориентированный язык). Вначале будет описан
формат формат шаблонов приавил, затем существующие правила будут описываться в виде выражений на этом DSL.


### Формат описания правил



#### Атомы

Атомы могут быть следующими:

 - **T** - токен;
 - **L** - список выражений;
 - **V** - значение;
 - **S** - что-то не до конца определенное, но соответствующее базовому синтаксису
           [S-выражений](https://ru.wikipedia.org/wiki/S-%D0%B2%D1%8B%D1%80%D0%B0%D0%B6%D0%B5%D0%BD%D0%B8%D0%B5);
 - **A** - значение или список значений
 - **;** - разделитель элементов списка атомов.


#### Токены T

После атома токена `T` может следовать один из модификаторов маркера токена:

 - **+** - один или больше элементов;
 - **?** - один элемент или ни одного;
 - __*__ - любое количество элементов;
 - **_** - допустимо пустое значение;
 - **i** - регистр токенов игнорируется;
 - **s** - токены задаются с учётом регистра;
 - **!** - нормальный останов разбора (без генерации ошибки) - дальнейший разбор будет производится 
           другими, детализирующими правилами. Это сделано для упрощения DSL, чтобы не вводить рекурсивные правила 
           и не усложнять разбор DSL.
 - **$** - остановка только в конце строки

Примеры:
```
T:a,b,c;
Ti:a,b,c; - игнорируем регистр (ignore case)
Ts:a,b,c; - сравнение с учетом регистра (case sensitive)
T!:a,b,c; - нормальный останов
```

#### Списки L

Список должен быть корректным [S-выражением](https://ru.wikipedia.org/wiki/S-%D0%B2%D1%8B%D1%80%D0%B0%D0%B6%D0%B5%D0%BD%D0%B8%D0%B5),
больше от него ничего не требуется. Его разбор будет производится позже.


#### Значения V

После атома значения `V` может следовать модификатор `0`, который обозначает, что допустимо отсутствие значения.

Значения могут быть заключены в кавычки, но кавычки не обязательны. Если значение содержит пробел, то оно должно быть 
заключено в кавычки. Кавычка внутри значения допустима и не требует экранирования, окончанием закавыченного значения
является кавычка и следующий за ней пробел. Никаких способов экранирования кавычек не предусмотрено.
Кавычка и следующий за ней пробел внутри значения не допустимы, но такие последовательности крайне маловероятно
могут понадобится в пользовательских правилах. Если понадобятся, будем думать.


#### S-атомы

Атомы `S` обозначают точно неопределённое на текущем уровне разбора, но корректное 
[S-выражение](https://ru.wikipedia.org/wiki/S-%D0%B2%D1%8B%D1%80%D0%B0%D0%B6%D0%B5%D0%BD%D0%B8%D0%B5).

После атома `S` может следовать модификатор `'` (одиночная кавычка), который обозначает, что не требуется
раскавычивание (unquoting).


#### Значение или список значений A

После атома `A` может следовать 
модификатор `'` (одиночная кавычка), который обозначает, что не требуется раскавычивание (unquoting); 
модификатор `0` - обозначает допустимость пустого значения;
модификатор `*` - обозначает допустимость отсутствия значений;


A0/A*/A'



```
// Пока хз, что это, надо вспоминать

            case ':':
            case ';':
                          flags |= (readedNumItemsVal&ExpressionParsingRuleFlags::number_mask) | ExpressionParsingRuleFlags::number_flag;

            case '+': flags |= ExpressionParsingRuleFlags::one_more_item_flag;

            case '?': flags |= ExpressionParsingRuleFlags::optional_item_flag;

            case '*': flags |= ExpressionParsingRuleFlags::any_number_items_flag;

            case '_': flags |= ExpressionParsingRuleFlags::empty_allowed_flag;

            case 'i': flags |= ExpressionParsingRuleFlags::icase_flag;

            case 's': flags &= ~ExpressionParsingRuleFlags::icase_flag;

            case '!': flags |= ExpressionParsingRuleFlags::good_stop_flag;

            case '$': flags |= ExpressionParsingRuleFlags::stops_on_eol_only;

            case '0': case '1': case '2': case '3': case '4':
            case '5': case '6': case '7': case '8': case '9':
                      if (readedNumber)
                          throw std::runtime_error("Expression parsing template - number taken multiple times");
                      readingNumber = true;

                      readedNumItemsVal *= 10;
                      readedNumItemsVal += (unsigned)(*pFlags-'0');
                      break;

```








ROBOCONF_PARSE_READ_LIST_BY_TEMPLATE( "Ti!:componentclass;M1$:set,assembly,match,matchTo"





