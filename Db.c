#include "Db.h"
#include <string.h>

static int record_cmp( const void *a, const void *b )
{
    Record *record_a = (Record *)a;
    Record *record_b = (Record *)b;
    return strcmp( record_a->brand, record_b->brand );
}

void record_free( Record *record )
{
   free( record->brand );
   free( record->founder );

   free( record );
}

void record_show( const Record *record )
{
    printf( "|%30s|%20s|%5d|\n",
            record->brand, record->founder, record->year_founded );
    return;
}

Record *record_new( const char *brand, const char *founder,
                    int year_founded )
{
    Record *guitar = malloc( sizeof( Record ) );
    if ( guitar == NULL ) return NULL;

    guitar->brand = strdup( brand );
    guitar->founder = strdup( founder );
    guitar->year_founded = year_founded;

    return guitar;
}

Database *database_new()
{
    Database *db = malloc( sizeof( Database ) );
    if ( !db ) return NULL;

    if ( db ) db->tree = tree_new( record_cmp, (void*)record_free );
    if ( !db->tree ) return NULL;

    return db;
}

bool database_insert( Database *db, Record *inserted_record )
{
    return tree_insert( db->tree, inserted_record );
}

Record *database_search( Database *db, const char *brand )
{
    Record *temp_record = record_new( brand, NULL, 0 );
    Record *founded_record = tree_search( db->tree, temp_record );

    record_free( temp_record );

    return founded_record;
}

void database_remove( Database *db, Record *record )
{
    tree_delete( db->tree, record );
}

void database_list( const Database *db )
{
    tree_walk( db->tree, (const void*)record_show );
}

int database_get_rows_count( const Database *db )
{
    return db->tree->size;
}
