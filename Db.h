#include "Tree.h"

typedef struct {
    char *brand;
    char *founder;
    int year_founded;
} Record;

Record *record_new( const char *brand, const char *founder,
                    int age );

// вывод содержимого записи на экран
void record_show( const Record *record );

void record_free( Record *record );

//---

// метод реализации база данных с использованием дерева
typedef struct {
    Tree *tree;
} Database;

Database *database_new();

bool database_insert( Database *db, Record *inserted_record );

// поиск записи по одному из полей
Record *database_search( Database *db, const char *brand );

// вывод содержимого бд в сортированном по возрастанию виде
void database_list( const Database *db );

// удаление записи из бд
void database_remove( Database *db, Record *record );

int database_get_rows_count( const Database *db );

// TODO удаление бд с освобождением памяти
// void database_drop( Database *db );

// database_serialize( const Database *db );
// database_deserialize_from_file( FILE *fp );


