# SearchServer / Поисковая система
*поработать с классом SearchServer, который позволяет выполнять поиск в базе документов*
## Интерфейс
### Конструктор
Конструктор класса SearchServer принимает поток ввода, содержащий базу документов. При этом:
- один документ — это одна строка входного потока;
- документы состоят из слов, разделённых одним или несколькими пробелами;
- слова состоят из строчных латинских букв.
### Метод AddQueriesStream(istream& query_input, ostream& search_results_output)
Метод AddQueriesStream выполняет поиск. Он принимает входной поток поисковых запросов и выходной поток для записи результатов поиска. При этом:
- один запрос — это одна строка в потоке query_input
- каждый поисковый запрос состоит из слов, разделённых одним или несколькими пробелами
- так же, как и в документах, слова в запросах состоят из строчных латинских букв

Результатом обработки поискового запроса является набор из максимум пяти наиболее релевантных документов. В реальных поисковых системах метрика релевантности устроена довольно сложно. В рамках задачи в качестве метрики релевантности используется суммарное количество вхождений всех слов запроса в документ. 
#### Пример:
Есть поисковая база из трёх документов: "london is the capital of great britain", "moscow is the capital of the russian federation", "paris is the capital of france", — и поисковый запрос "the best capital". Тогда метрика релевантности у документов будет такой:
- london is the capital of great britain — 2 (слово "the" входит в документ 1 раз, слово "best" — ни разу, слово "capital" — 1 раз)
- moscow is the capital of the russian federation — 3 (слово "the" входит в документ 2 раза, слово "best" — ни разу, слово "capital" — 1 раз)
- paris is the capital of france — 2 ("the" — 1, "best" — 0, "capital" — 1)

В итоге получается, что документ "moscow is the capital of the russian federation" оказывается наиболее релевантным запросу "the best capital".

Для каждого поискового запроса метод AddQueriesStream должен вывести в поток search_results_output одну строку в формате [текст запроса]: {docid: <значение>, hitcount: <значение>} {docid: <значение>, hitcount: <значение>} ..., где docid — идентификатор документа (см. ниже), а hitcount — значение метрики релевантности для данного документа (то есть суммарное количество вхождений всех слов запроса в данный документ).

Два важных замечания:
- Добавлять в результаты поиска документы, hitcount которых равен нулю, не нужно.
- при подсчёте hitcount нужно учитывать только слова целиком, то есть слово «there» не является вхождением слова «the»

### Метод UpdateDocumentBase(istream& document_input)
Метод UpdateDocumentBase заменяет текущую базу документов на новую, которая содержится в потоке document_input. При этом документ из первой строки этого потока будет иметь идентификатор (docid) 0, документ из второй строки — идентификатор 1 и т.д. Точно так же должен назначать идентификаторы документам и конструктор класса SearchServer. 

## Первая часть задачи
- document_input содержит не более 50000 документов
- каждый документ содержит не более 1000 слов
- общее число различных слов во всех документах не превосходит 15000
- максимальная длина одного слова — 100 символов, слова состоят из строчных латинских букв и разделены одним или несколькими пробелами
- query_input содержит не более 500 000 запросов, каждый запрос содержит от 1 до 10 слов.

## Вторая часть задачи
web-сервер бо̀льшую часть времени обрабатывает поисковые запросы. База документов обновляется не очень часто с определённой периодичностью, например, один раз в сутки. Обновление базы может занимать значительное время, поэтому сервер не должен прекращать обработку запросов, пока оно выполняется. Решение второй части задачи должно учитывать:
- Метод AddQueriesStream должен быть готов к тому, что параллельно с ним будет выполняться метод UpdateDocumentBase и наоборот.
- Метод AddQueriesStream не обязан обрабатывать все запросы с той версией базы документов, которая была актуальна во время его запуска. То есть, если во время работы метода AddQueriesStream база документов была обновлена, он может использовать новую версию базы для обработки оставшихся запросов.
