#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <vector>
#include <memory>
#include <algorithm>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include  <io.h>
#include  <stdio.h>
#include  <stdlib.h>
/*
This file is part of WSLCLI.

	Copyright 2012 Christopher Augustus Greeley

    WSLCLI is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    WSLCLI is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with WSLCLI.  If not, see <http://www.gnu.org/licenses/>.
*/


using namespace std;


bool stb( string toBool )
{
    if( toBool != "0" && toBool != "false" )
        return ( true );
    return ( false );
}

string bts( bool toString ) {
    if( toString )
        return { "true" };
    return { "false" };
}

string operator + ( string a, string b )
{
    stringstream product;
    product << a << b;
    return product.str();
}
string operator + ( string a, unsigned int b )
{
    stringstream product;
    product << a << b;
    return product.str();
}

string operator + ( string a, bool b )
{
    stringstream product;
    product << a << bts( b );
    return product.str();
}

static const string SCRIPT_LINE = "----------";
static const string DEFAULT_SCRIPT_VALUE = "NULL";
static const string DEFAULT_ENTITY_NAME = "Object";
static const string NO_OPTION = "No option selected.";
static const string CURRENTLY = "Currently, ";
static const string QUIT = "Quit";
static string obj, lev;
static const char numbers[10] = { '0', '1', '2', '3', '4',
    '5', '6', '7', '8', '9'
};

//To make it easier to write "input entity" later on.//
enum ENTITY_NUMARICAL_EQUIVLILENCE
{
    COMP_ID = 0,
    OBJ_ID = 1,
    INIT = 2,
    REF = 3,
    DES = 4,
    DES_ON_CREATE = 5,
    INIT_SCR = 6,
    REF_SCR = 7,
    DES_SCR = 8,
    PHYS_SCR = 9,
};


struct Entity
{
    unsigned int objID, compID;
    string initScript, refScript, physScript, desScript;
    bool init, ref, des, desOnCreate;
    string name;
};

struct DoubleString
{
    string a, b;
    DoubleString( string a_ = "", string b_ = "" ) {
        a = a_;
        b = b_;
    }
};

typedef DoubleString OPTION;
typedef vector< DoubleString > MENU;

/*Set all the entitie's values to defult values,
the reason it is in a function is because it would
look ugly as a constructor.*/
Entity* DefaultEntityInitialize( Entity *entity )
{
    entity->compID = 0;
    entity->des = false;
    entity->desOnCreate = false;
    entity->desScript = DEFAULT_SCRIPT_VALUE;
    entity->init = false;
    entity->initScript = DEFAULT_SCRIPT_VALUE;
    entity->name = DEFAULT_ENTITY_NAME;
    entity->objID = 0;
    entity->physScript = DEFAULT_SCRIPT_VALUE;
    entity->ref = false;
    entity->refScript = DEFAULT_SCRIPT_VALUE;
    return entity;
}

//Yikes!//
string DisplayMenu( MENU toDisplay )
{
    string input{ "" };
    bool notValid = true;
    while( notValid )
    {
        bool break__ = false;
        for( unsigned int i = 0; i < toDisplay.size(); ++i )
        {
            cerr << i << ": " << toDisplay[ i ].a;
            if( toDisplay[ i ].b != "" )
                cerr << " : " << toDisplay[ i ].b;
            cerr << endl;
        }
        cerr << "\n\nPlease input the respective number of your choice.\n--->\t";
        getline( cin, input );
        for( unsigned int j = 0; j < input.size() && break__ == false; ++j )
        {
            for( unsigned int o = 0; o < 10; ++o )
            {
                if( input[ j ] == numbers[ o ] ) {
                    notValid = false;
                    break;
                }
                else
                {
                    notValid = true;
                    if( o == 9 )
                    {
                        cerr << "You have entered invalid input, please only enter a number.\n";
                        break__ = true;
                        break;
                    }
                }
            }
        }
        if( ( unsigned int ) atoi( input.c_str() ) >= toDisplay.size() && notValid == false ) {
            cerr << "You have entered invalid input, please only enter a respective number.\n";
            notValid = true;
        }
    }
    return input;
}

void MakeDir()
{
    string input;
    std::stringstream buffer, level, object;
    cerr << "Please enter the (full) directory of your project: \n";
    getline( cin, input );
    cerr << input << "\n";
    if( input[ input.size() - 1 ] != '/' && input[ input.size() - 1 ] != '\\' )
        buffer << input << "/";
    else
        buffer << input;
    buffer << "bin/Info/";
    object << buffer.str() << "Objects/";
    level << buffer.str() << "Levels/";
    obj = object.str();
    lev = level.str();
}

void Clear() {
    system( "cls" );
}
/*Example:
1
1
true
true
false
false
----------
src/Scripts/Objects/Ship/Initialize.lua
----------
src/Scripts/Objects/Ship/Refresh.lua
----------
NULL
----------
NULL
----------
src/Scripts/Objects/Ship/Physics.lua
*/

void WriteBool( stringstream *writeTo, bool value )
{
    if( value == true )
        *writeTo << "true\n";
    else
        *writeTo << "false\n";
}

void WriteScript( stringstream *writeTo, string script, string lineEnding ) {
    *writeTo << SCRIPT_LINE << "\n";
    *writeTo << script << lineEnding;
}

void WriteEntityToFile( Entity *toWrite )
{
    ofstream file;
    stringstream entity, fileName, cmd, fileDirectory, mkdir;
    entity << toWrite->objID << "\n";
    entity << toWrite->compID << "\n";
    WriteBool( &entity, toWrite->init );
    WriteBool( &entity, toWrite->ref );
    WriteBool( &entity, toWrite->des );
    WriteBool( &entity, toWrite->desOnCreate );
    WriteScript( &entity, toWrite->initScript, "\n" );
    WriteScript( &entity, toWrite->refScript, "\n" );
    WriteScript( &entity, toWrite->desScript, "\n" );
    WriteScript( &entity, "NULL", "\n" );
    if( toWrite->compID == 0 )
        WriteScript( &entity, "NULL", "" );
    else
        WriteScript( &entity, toWrite->physScript, "" );
    fileDirectory << obj << toWrite->name << "/";
    fileName << fileDirectory.str() << "Object.txt";
    mkdir << 'm' << 'k' << 'd' << 'i' << 'r' << ' ' << '"' << fileDirectory.str() << '"';
    cmd << "del " << fileName.str();
    system( cmd.str().c_str() );
    system( mkdir.str().c_str() );
    //Clear();
    file.open( fileName.str().c_str() );
    try
    {
        if( file.good() && file.is_open() )
            file << entity.str().c_str();
        else
            throw "Failed to write to file!";
    }
    catch( string e ) {
        cerr << e << "\n";
    }
    file.close();
}

Entity* ReadEntity( Entity *toWrite )
{
    string file = "";
    file = obj + toWrite->name + "/" + "Object.txt";
    ifstream entityFile;
    entityFile.open( file.c_str() );
    vector< string > scripts = { DEFAULT_SCRIPT_VALUE, DEFAULT_SCRIPT_VALUE,
        DEFAULT_SCRIPT_VALUE, DEFAULT_SCRIPT_VALUE, DEFAULT_SCRIPT_VALUE
    };
    string buffer = "";
    for( unsigned short i = 0; i < 2; ++i )
    {
        getline( entityFile, buffer );
        switch( ( ENTITY_NUMARICAL_EQUIVLILENCE ) i )
        {
            //Beacuse in the files, object ID comes first.//
            case COMP_ID:
                toWrite->objID = atoi( buffer.c_str() );
                break;
            default:
                toWrite->compID = atoi( buffer.c_str() );
                break;
        }
    }
    for( unsigned short i = 0; i < 4; ++i )
    {
        getline( entityFile, buffer );
        switch( ( ENTITY_NUMARICAL_EQUIVLILENCE ) ( i + 2 ) )
        {
            case INIT:
                toWrite->init = stb( buffer );
                 break;
            case REF:
                toWrite->ref = stb( buffer );
                break;
            case DES:
                toWrite->des = stb( buffer );
                break;
             case DES_ON_CREATE:
                toWrite->desOnCreate = stb( buffer );
                break;
        }
    }
     for( unsigned short i = 0; entityFile.eof() != true; ++i )
     {
        getline( entityFile, buffer );
        if( i == 1 )
                toWrite->initScript = buffer;
        if( i == 5 )
                toWrite->desScript = buffer;
        //Skip one!//
        if( i == 9 )
                toWrite->physScript = buffer;
        if( i == 3 )
                toWrite->refScript = buffer;
    }
    return toWrite;
}

void Will( string action, bool condition )
{
    if( condition )
        cerr << "Will '" << action << "'.\n";
    else
        cerr << "Will not '" << action << "'.\n";
}

void PrintEntity( Entity *toPrint )
{
    cerr << "Type: ";
    switch( toPrint->compID )
    {
        case 0:
            cerr << "Entity Component ID: 0";
            break;
        case 1:
            cerr << "Positional Component ID: 1";
            break;
        case 2:
            cerr << "Mobile Component ID: 2";
            break;
        default:
            cerr << "User Specified Type Component ID: " << toPrint->compID;
    }
    cerr << endl;
    cerr << "Object ID: " << toPrint->objID << "\n";
    Will( "initialize", toPrint->init );
    Will( "refresh", toPrint->ref );
    Will( "destroy", toPrint->des );
    Will( "destroy on creation", toPrint->desOnCreate );
    cerr << "Initialize Script: " << toPrint->initScript << "\n";
    cerr << "Refresh Script: " << toPrint->refScript << "\n";
    cerr << "Destroy Script: " << toPrint->desScript << "\n";
    if( toPrint->compID )
    cerr << "Physical Behavior Script: " << toPrint->physScript << "\n";
    cerr << "File Name: " << toPrint->name << "\n";
}


bool IsNumber( string sample )
{
    for( unsigned int i = 0; i < sample.size(); ++i )
    {
        bool notANumber = true;
        for( unsigned int j = 0; j < 10; ++j )
        {
            if( sample[ i ] == numbers[ j ] ) {
                notANumber = false;
                break;
            }
            if( ( j + 1 ) == 10 &&  sample[ i ] != numbers[ j ] )
                return ( true );
        }
        if( ( i + 1 ) == sample.size() )
            return ( false );
    }
}


MENU CopyMenu( MENU template__, MENU into )
{
    for( unsigned int i = 0; i < template__.size() && i < into.size(); ++i ) {
        into[ i ].a = template__[ i ].a;
        into[ i ].b = template__[ i ].b;
    }
    return into;
}


template < typename T >
class MainMenu
{
    MENU mainMenu;
    public:
        MainMenu()
        {
            mainMenu = {
                OPTION( "Edit Entity" ), OPTION( "Create Entity" ), OPTION( "QUIT" )
            };
        }
        T* ExecuteEntityEditor( T* editor )
        {
            bool stop = false;
            while( stop == false )
                stop = editor->Display();
            return editor;
        }
        bool HandleMenu()
        {
            int option = atoi( DisplayMenu( mainMenu ).c_str() );
            if( option == 0 )
                ChooseEntity();
            else if( option == 1 )
            {
                T *editor = new T( true );
                editor->SetMenu();
                ExecuteEntityEditor( editor );
            }
            else
                return false;
            return true;
        }

        void ChooseEntity()
        {
            string entityName;
            stringstream cmd;
            cmd << "dir ";
            cmd << '"';
            Entity *e = new Entity;
            e = DefaultEntityInitialize( e );
            for( unsigned int i = 0; i < obj.size() - 1; ++i )
                cmd << obj[ i ];
            cmd << '"';
            system( cmd.str().c_str() );
            cerr << "\nPlease enter the name of the entity you would like to edit.\n";
            getline( cin, entityName );
            e->name = entityName;
            T *use = new T( true );
            use->SetEditing( ReadEntity( e ) );
            use->SetMenu();
            use = ExecuteEntityEditor( use );
            T::CleanUp();
        }

};

class EntityEditor
{
    Entity *editing;
    bool destroy;
    int id;
    string inputBuffer, currentOption;
    MENU withPhysics, withoutPhysics;
    public:
        static MENU defaultMenu;
        static vector< unique_ptr< EntityEditor > > editors;
        EntityEditor( bool init = false, Entity *editing_ = new Entity )
        {
            //So this does not happen every time the editor may be passed around.//
            if( init )
            {
                id = editors.size() - 1;
                currentOption = NO_OPTION;
                withoutPhysics = defaultMenu;
                editing = editing_;
                editing = DefaultEntityInitialize( editing );
                destroy = false;
                editors.push_back( unique_ptr< EntityEditor >( this ) );
                WriteEntityToFile( editing );
            }
        }
        //Clean up.//
        ~EntityEditor()
        {
            //So it does not happen every time a copy of the an editor is destroyed.//
            if( destroy ) {
                destroy = false;
                delete editing;
            }
        }

        void SetMenu()
        {
            withoutPhysics[ 3 ].b = withoutPhysics[ 3 ].b + editing->name;
            withoutPhysics[ 4 ].b = withoutPhysics[ 4 ].b + editing->compID;
            withoutPhysics[ 5 ].b = withoutPhysics[ 5 ].b + editing->objID;
            withoutPhysics[ 6 ].b = withoutPhysics[ 6 ].b + editing->init;
            withoutPhysics[ 7 ].b = withoutPhysics[ 7 ].b + editing->ref;
            withoutPhysics[ 8 ].b = withoutPhysics[ 8 ].b + editing->des;
            withoutPhysics[ 9 ].b = withoutPhysics[ 9 ].b + editing->desOnCreate;
            withoutPhysics[ 10 ].b = withoutPhysics[ 10 ].b + editing->initScript;
            withoutPhysics[ 11 ].b = withoutPhysics[ 11 ].b + editing->refScript;
            withoutPhysics[ 12 ].b = withoutPhysics[ 12 ].b + editing->desScript;
            withPhysics = withoutPhysics;
            withPhysics.push_back( OPTION( "Physics Script (Directoy, File Name of A .lua File)", "Currently, " + editing->physScript ) );

        }

        //Clean up everything.//
        static void CleanUp()
        {
            unsigned int originalSize = EntityEditor::editors.size();
            for( unsigned int i = 0; i < originalSize; ++i ) {
                if( EntityEditor::editors[ 0 ]->GetDestroy() == true )
                    EntityEditor::editors.erase( EntityEditor::editors.begin() );
            }
        }
                //Clean up everything.//
        static void CleanAll()
        {
            unsigned int originalSize = EntityEditor::editors.size();
            for( unsigned int i = 0; i < originalSize; ++i )
                EntityEditor::editors.erase( EntityEditor::editors.begin() );
        }

        bool Display()
        {
            switch( editing->compID )
            {
                case 0:
                    currentOption = DisplayMenu( withoutPhysics );
                    break;
                default:
                    currentOption = DisplayMenu( withPhysics );
                    break;
            }
            UpdateOptions();
            return destroy;
        }
        void UpdateOptions()
        {
            unsigned int option = atoi( currentOption.c_str() );
            if( option == 0 )
            {
                Clear();
                PrintEntity( editing );
                cin.get();
                return;
            }
            if( option == 1 )
            {
                Clear();
                cerr << "This is the data that will be written to a file.\n";
                PrintEntity( editing );
                cin.get();
                WriteEntityToFile( editing );
                destroy = true;
                return;
            }
            if( option == 2 )
            {
                getline( cin, inputBuffer );
                SetEntityName( inputBuffer );
                return;
            }
            if( option == 3 )
            {
                getline( cin, inputBuffer );
                string originalName = editing->name;
                editing->name = inputBuffer;
                system( ( "del " + obj + originalName + "/" + originalName + ".txt" ).c_str() );
                WriteEntityToFile( editing );
                withoutPhysics[ 2 ].b = CURRENTLY + editing->name;
                withPhysics[ 2 ].b = CURRENTLY + editing->name;
                return;
            }
            getline( cin, inputBuffer );
            if( editing->compID == 0) {
                withoutPhysics[ option ].b = CURRENTLY + InputToEntity( ( ENTITY_NUMARICAL_EQUIVLILENCE ) ( option - 4 ), inputBuffer );
                withPhysics = CopyMenu( withoutPhysics, withPhysics );
            }
            else {
                withPhysics[ option ].b = CURRENTLY + InputToEntity( ( ENTITY_NUMARICAL_EQUIVLILENCE ) ( option - 4 ), inputBuffer );
                withoutPhysics = CopyMenu( withPhysics, withoutPhysics );
            }
        }
        string InputToEntity( ENTITY_NUMARICAL_EQUIVLILENCE toWhat, string input )
        {
            switch( toWhat )
            {
                case COMP_ID:
                    editing->compID = atoi( input.c_str() );
                    break;
                case OBJ_ID:
                    editing->objID = atoi( input.c_str() );
                    break;
                case INIT:
                    if( input == "true" )
                        editing->init = true;
                    else
                        editing->init = false;
                    break;
                case REF:
                    if( input == "true" )
                        editing->ref = true;
                    else
                        editing->ref = false;
                    break;
                case DES:
                    if( input == "true" )
                        editing->des = true;
                    else
                        editing->des = false;
                    break;
                case DES_ON_CREATE:
                    if( input == "true" )
                        editing->desOnCreate = true;
                    else
                        editing->desOnCreate = false;
                    break;
                case INIT_SCR:
                    editing->initScript = input;
                    break;
                case REF_SCR:
                    editing->refScript = input;
                    break;
                case DES_SCR:
                    editing->desScript = input;
                    break;
                case PHYS_SCR:
                    editing->physScript = input;
                    break;
            }
            return input;
        }
        Entity* GetEditing() {
            return editing;
        }
        bool GetDestroy() {
            return destroy;
        }
        int GetID() {
            return id;
        }
        void SetDestroy( bool destroy_ ) {
            destroy = destroy_;
        }
        void SetEntityName( string name ) {
            editing->name = name;
            editing = ReadEntity( editing );
        }
        void SetEditing( Entity *editing_ ) {
            delete editing;
            editing = editing_;
        }
};

MENU EntityEditor::defaultMenu =
{
                OPTION ( "Print Entity" ), OPTION( "Go To Main Menu" ), OPTION( "DEPROCATED: Read Entity From File (Folder Name)" ),
                OPTION( "Entity Name", CURRENTLY ), OPTION( "Edit Component ID (Number)", CURRENTLY ),
                OPTION( "Edit Object ID (Number)", CURRENTLY ), OPTION( "Set Initialize (true, false)", CURRENTLY ),
                OPTION( "Set Refresh (true, false)", CURRENTLY ), OPTION( "Set Destroy (true, false)", CURRENTLY ),
                OPTION( "Set Destroy On Create (true, false)", CURRENTLY ), OPTION( "Initialize Script (Directoy, File Name of A .lua File)", CURRENTLY ),
                OPTION( "Refresh Script (Directoy, File Name of A .lua File)", CURRENTLY ), OPTION( "Destroy Script (Directoy, File Name of A .lua File)", CURRENTLY )
};
vector< unique_ptr< EntityEditor > > EntityEditor::editors;


int main()
{
    system( "color 70" );
    MakeDir();
    MainMenu< EntityEditor > m;
    while( m.HandleMenu() );
    EntityEditor::CleanAll();
    return 0;
}
