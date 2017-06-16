#include "KCommandLineTokenizer.hh"
#include "KInitializationMessage.hh"

#include <cstring>

using namespace std;

extern char** environ;

namespace katrin
{

    KCommandLineTokenizer::KCommandLineTokenizer()
    {
    }
    KCommandLineTokenizer::~KCommandLineTokenizer()
    {
    }

    void KCommandLineTokenizer::ProcessCommandLine( int anArgC, char** anArgV )
    {
        if( anArgC <= 1 )
        {
            return;
        }

        char** env;
        string tVariableName;
        string tVariableValue;

        for (env = environ; *env != 0; env++)
        {
            string tEnv(*env);
            stringstream env_stream(tEnv);
            getline(env_stream, tVariableName, '=');
            getline(env_stream, tVariableValue, '=');

            fVariables[tVariableName] = tVariableValue;
        }


        char** tArgument = anArgV;
        int tArgumentCount = 1;
        string tFileName;
        while( strcmp( tArgument[tArgumentCount], "-r" ) != 0 )
        {
            tFileName = string( tArgument[tArgumentCount] );
            fFiles.push_back( tFileName );

            initmsg_debug( "adding file named <" << tFileName << ">" << eom );

            tArgumentCount++;

            if( tArgumentCount == anArgC )
            {
                return;
            }
        }

        if( strcmp( tArgument[tArgumentCount], "-r" ) == 0 )
        {
            tArgumentCount++;
        }

        string tVariableDescription;
        size_t tVariableEqualPos;

        while( tArgumentCount < anArgC )
        {
            tVariableDescription = string( tArgument[tArgumentCount] );
            if( tVariableDescription.length() < 3 )
            {
                initmsg( eError ) << "could not interpret command line argument <" << tVariableDescription << ">" << eom;
                return;
            }
            tVariableEqualPos = tVariableDescription.find( '=' );
            tVariableName = tVariableDescription.substr( 0, tVariableEqualPos );
            tVariableValue = tVariableDescription.substr( tVariableEqualPos + 1 );
            fVariables[tVariableName] = tVariableValue;

            initmsg_debug( "defining variable named <" << tVariableName << "> with value <" << tVariableValue << ">" << eom );

            tArgumentCount++;
        }


        return;
    }

}
