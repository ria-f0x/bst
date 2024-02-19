#include <Db.h>

int main( void )
{
    Database *db = database_new();

    Record *peavey = record_new(
                "Peavey",
                "Hartley Peavey",
                1965 );
    Record *gibson = record_new(
                "Gibson Guitar Corporation",
                "Orvillie Gibson",
                1902 );
    Record *charvel = record_new(
                "Charvel",
                "Wayne Charvel",
                1974 );
    Record *music_man = record_new(
                "Music Man",
                "Leo Fender",
                1974 );

    database_insert( db, peavey );
    database_insert( db, gibson );
    database_insert( db, charvel );
    database_insert( db, music_man );

    database_list( db );

    Record *founded_record = database_search( db, "Charvel" );

    if ( founded_record ) database_remove( db, founded_record );

    database_list( db );

    return 0;
}
