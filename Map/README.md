### Map hints

Использование hint в методе insert даёт существенный прирост скорости в алгоритмах, когда
* элементы поступают в порядке возрастания / убывания
* элементы представляют собой последовательность с увеличивающимся диапазоном
* элементы представляют собой сходящуюся последовательность

В целом это всё довольно синтетические примеры.

К реальности наиболее приближен вот такой случай:

Пусть у нас есть таблица пользователей (class FakeDatabase), в которой строки отсортированы по имени пользователя.\
Мы хотим считать данные из этой таблицы в мапу, ключом которой является имя пользователя.\
Если использовать предположение о том, что строки в таблице уже отсортированы в лексикографическом порядке, тогда создание мапы можно значительно ускорить.

В коде приведены примеры создания мапы без хинтов, с хинтами при проходе данных по возрастанию и при проходе данных по убыванию.\
Доступ к элементам получившихся мап примерно одинаков (см. результаты функции check).