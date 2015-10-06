#include <iostream>
#include <fstream>
#include <cmath>
#include <vector>
#include <string>
#include <sstream>
#include <dirent.h>
#include <iomanip>


using namespace std;

void SetDataStream( string, std::stringstream&);
bool DirectoryExists( const char* pzPath );

int main(int argc, char **argv)
{
    string fileName, outDirName;
    std::stringstream stream, numConv;
    vector<bool*> reactList[101];
    vector<string> isoNameList[101];
    vector<int> isoAList[101];
    string dirName[36]={"FS/","FC/","SC/","TC/","LC/","F01/","F02/","F03/","F04/","F05/","F06/","F07/","F08/","F09/","F10/","F11/","F13/","F14/","F15/","F18/","F19/","F20/","F21/","F22/","F23/","F24/","F25/","F26/","F27/","F28/","F30/","F31/","F33/","F34/","F35/","F36/"};
    int MTRList[36]={18,19,20,21,38,4,5,11,16,17,22,23,24,25,28,29,32,33,34,37,41,42,44,45,103,104,105,106,107,108,111,112,113,115,116,117};

    if(argc==2)
    {
        fileName=argv[1];
        outDirName=fileName.substr(0,fileName.find_last_of('/')+1);
    }
    else if(argc==3)
    {
        fileName=argv[1];
        outDirName=argv[2];
    }
    else
    {
        cout << "\nError: Incorrect number of inputs\nOption 1: Provide full path to G4NDL directory \nOption 2: Provide full path to G4NDL directory and the output directory" << endl;
        return 1;
    }

    outDirName = outDirName+"ReactionList/";
    if(!(DirectoryExists((outDirName).c_str())))
    {
        system( ("mkdir -p -m=777 "+outDirName).c_str());
        if(DirectoryExists((outDirName).c_str()))
        {
            cout << "created directory " << outDirName << "\n" << endl;
        }
        else
        {
            cout << "\nError: could not create Directory " << outDirName << "\n" << endl;
            return 1;
        }
    }

    if(fileName.back()!='/')
    {
        fileName.push_back('/');
    }

    int i=0, Z, A;
    DIR *dir;
    struct dirent *ent;
    string react = "Fission/", fullName;
    for(int index=0; index<36; index++)
    {
        if(index==5)
            react = "Inelastic/";
        fullName=fileName+react+dirName[index];
        if ((dir = opendir (fullName.c_str())) != NULL)
        {
            while ((ent = readdir (dir)) != NULL)
            {
                if((string(ent->d_name)!="..")&&(string(ent->d_name)!="."))
                {
                    i=0;
                    numConv.str("");
                    numConv.clear();
                    while((string(ent->d_name)[i]!='_')&&(string(ent->d_name).length()>i))
                    {
                        numConv << string(ent->d_name)[i];
                        i++;
                    }
                    if(i!=string(ent->d_name).length())
                    {
                        numConv >> Z;
                        i++;
                        numConv.str("");
                        numConv.clear();
                        while((string(ent->d_name)[i]!='_')&&(string(ent->d_name).length()>i))
                        {
                            numConv << string(ent->d_name)[i];
                            i++;
                        }
                        if(i!=string(ent->d_name).length())
                        {
                            if(numConv.str()=="nat")
                            {
                                if(isoNameList[Z].size()==0)
                                {
                                    isoNameList[Z].push_back(string(ent->d_name));
                                    reactList[Z].push_back(new bool [36]);
                                    for(int k=0; k<36; k++)
                                    {
                                        reactList[Z].back()[k]=0;
                                    }
                                    isoAList[Z].push_back(0);
                                }
                                reactList[Z][0][index]=1;
                            }
                            else
                            {
                                numConv >> A;
                                int j=0;
                                for(; j<isoNameList[Z].size(); j++)
                                {
                                    if(A==isoAList[Z][j])
                                    {
                                        break;
                                    }
                                }
                                if(isoNameList[Z].size()==j)
                                {
                                    j=0;
                                    for(; j<isoNameList[Z].size(); j++)
                                    {
                                        if(A<isoAList[Z][j])
                                        {
                                            break;
                                        }
                                    }
                                    isoNameList[Z].insert(isoNameList[Z].begin()+j, string(ent->d_name));
                                    reactList[Z].insert(reactList[Z].begin()+j, new bool [36]);
                                    for(int k=0; k<36; k++)
                                    {
                                        reactList[Z][j][k]=0;
                                    }
                                    isoAList[Z].insert(isoAList[Z].begin()+j, A);
                                }
                                reactList[Z][j][index]=1;

                            }
                        }
                    }
                }
            }
            closedir(dir);
        }
        else
        {
            cout << "Could not open " << fullName << endl;
        }
    }

    for(int i=0; i<101; i++)
    {
        for(int j=0; j<isoNameList[i].size(); j++)
        {
            stream.str("");
            stream.clear();
            stream << "Fission Reactions Present: ";
            for(int k=0; k<5; k++)
            {
                if(reactList[i][j][k])
                {
                    stream << MTRList[k] << " ";
                }
            }
            stream << "\n\nFission Reactions Not Present: ";
            for(int k=0; k<5; k++)
            {
                if(!reactList[i][j][k])
                {
                    stream << MTRList[k] << " ";
                }
            }

            stream << "\n\Inelastic Reactions Present: ";
            for(int k=5; k<36; k++)
            {
                if(reactList[i][j][k])
                {
                    stream << MTRList[k] << " ";
                }
            }
            stream << "\n\nInelastic Reactions Not Present: ";
            for(int k=5; k<36; k++)
            {
                if(!reactList[i][j][k])
                {
                    stream << MTRList[k] << " ";
                }
            }
            SetDataStream( string(outDirName+isoNameList[i][j]), stream);
            delete [] reactList[i][j];
        }
    }

    return 0;

}

bool DirectoryExists( const char* pzPath )
{
    if ( pzPath == NULL) return false;

    DIR *pDir;
    bool bExists = false;

    pDir = opendir (pzPath);

    if (pDir != NULL)
    {
        bExists = true;
        closedir (pDir);
    }

    return bExists;
}

void SetDataStream( string filename , std::stringstream& ss )
{
    //bool cond=true;
// Use regular text file
    if(filename.substr((filename.length()-2),2)==".z")
    {
        filename.pop_back();
        filename.pop_back();
    }

      std::ofstream out( filename.c_str() , std::ios::out | std::ios::trunc );
      if ( ss.good() )
      {
         ss.seekg( 0 , std::ios::end );
         int file_size = ss.tellg();
         ss.seekg( 0 , std::ios::beg );
         char* filedata = new char[ file_size ];
         while ( ss ) {
            ss.read( filedata , file_size );
            if(!file_size)
            {
                cout << "\n #### Warning the size of the stringstream for " << filename << " is zero ###" << endl;
                break;
            }
         }
         out.write(filedata, file_size);
         if (out.fail())
        {
            cout << endl << "### Error: writing the ascii data to the output file " << filename << " failed" << endl
                 << " may not have permission to delete an older version of the file ###" << endl;
        }
         out.close();
         delete [] filedata;
      }
      else
      {
// found no data file
//                 set error bit to the stream
         ss.setstate( std::ios::badbit );

         cout << endl << "### failed to write to ascii file " << filename << " ###" << endl;
      }
   ss.str("");
}
