#include <iostream>
#include <fstream>
#include <stdio.h>
#include <string>
#include<string.h>
#include <vector>
#include <algorithm>
#include <functional>
#include <bits/stdc++.h>
#include <stack>
#include <map>
#include <math.h>
#include <chrono>
#include <iostream>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>

struct stat info;


using namespace std::chrono;

using namespace std;

typedef struct node
{
    char letter;
    int frequency;
    struct node* left;
    struct node* right;
    int code;
} node;

int ASCII[256] = {0};
map<char, string> codes;

int Folder=0;
string totalFolder="";
int OriginalFileSize=0;
int CompressedFileSize=0;

void printCodes(node* root, string str)
{
    if (!root)
        return;
    if (root->letter != char(157))
    {
        std::bitset<8> y(root->letter);
        cout << root->letter << "\t\t" << y<< "\t\t" <<str << "\n";
        return;
    }
    printCodes(root->left, str + "0");
    printCodes(root->right, str + "1");
}

void storeCodes(node* root, string str)
{
    if (!root)
        return;
    if (root->letter != char(157))
    {
        codes[root->letter]=str;
        return;
    }
    storeCodes(root->left, str + "0");
    storeCodes(root->right, str + "1");
}


void deleteTree(node* root)
{
    if(root->left)
    {
        deleteTree(root->left);
        free(root->left);
    }
    if(root->right)
    {
        deleteTree(root->right);
        free(root->right);
    }

}


void compressFile(char filename[])
{
    string total="";
    vector <char> encoded;
    ifstream newfile;
    ofstream outfile;
    int zeros;

    newfile.open(filename,ios::in);
    char cr;

    while(newfile.get(cr))
    {
        total+=codes[cr];
        if (newfile.eof())
        {
            return;
        }
    }
    newfile.close();

    if(total.size()%8!=0)
    {
        zeros=8-(total.size()%8);
        for(int j=0; j<zeros; j++)
        {
            total+="0";
        }
    }


    outfile.open("compressed.txt",ios::out);
    map<char, string>::iterator it;

    outfile << zeros;

    for (it = codes.begin(); it != codes.end(); it++)
    {
        outfile << it->first << char(236);
        outfile << it->second << char(153);
        outfile.flush();
    }
    outfile << char(234);
    stringstream sread(total);


    while(!sread.eof())
    {
        bitset<8>bit;
        sread>>bit;
        long dec = bit.to_ulong();
        encoded.push_back(char(dec));
        outfile.flush();
    }

    for(int k=0; k<encoded.size()-1; k++)
    {
        outfile << encoded.at(k);
    }

    outfile.close();

}

bool compareNode(node* a, node* b)
{
    return (a->frequency < b->frequency);
}


void BuildTree(int* ASCII)
{
    vector<node*> pq;
    for(int i=0; i<256; i++)
    {
        if(ASCII[i]!=0)
        {
            node* newNode=(node*)malloc(sizeof(node*));
            newNode->letter=char(i);
            newNode->frequency=ASCII[i];
            newNode->left=NULL;
            newNode->right=NULL;
            pq.push_back(newNode);
            std::sort(pq.begin(), pq.end(), compareNode);
        }
    }

    while(pq.size()>1)
    {
        node* first =(node*)malloc(sizeof(node*));
        first=  pq.front();
        pq.erase(pq.begin());
        node* second =(node*)malloc(sizeof(node*));
        second=pq.front();
        pq.erase(pq.begin());
        node* sum=(node*)malloc(sizeof(node*));
        sum->letter=char(157);
        sum->frequency=first->frequency+second->frequency;
        sum->left=first;
        sum->right=second;
        pq.push_back(sum);
        std::sort(pq.begin(), pq.end(), compareNode);
    }
    cout << "Byte\t\t Code\t\t      New Code\n";
    string code="";
    printCodes(pq.front(),code);
    storeCodes(pq.front(),code);
    deleteTree(pq.front());

}

void ReadInputfile(char filename[])
{
    ifstream newfile;
    newfile.open(filename,ios::in);
    if(!newfile)
    {
        printf("Invalid Destination");
        exit(0);
    }
    char c;

    while(newfile.get(c))
    {
        ASCII[c]++;
    }
    newfile.close();

}

void decompress()
{
    ifstream newfile;
    newfile.open("compressed.txt",ifstream::in);

    char paded0;
    newfile.get(paded0);
    char cr;
    char temp;
    newfile.get(cr);
    temp=cr;
    while(cr!=char(234))
    {
        string temp2="";
        if(cr==char(236))
        {
            newfile.get(cr);
            while(cr!=char(153))
            {
                codes[temp]+=cr;
                newfile.get(cr);
            }
        }
        else
        {
            temp=cr;
            newfile.get(cr);

        }
    }
    string total="";
    while(newfile.get(cr))
    {
        bitset<8> bit(cr);
        total+=bit.to_string();
    }
    string bit="",output="";
    map<string, char> reversecodes;

    for(map<char, string>::iterator i = codes.begin(); i!=codes.end(); i++)
    {
        reversecodes[i->second] = i->first;
    }

    int PaddedZero = paded0 - '0';

    for(int i=0; i<total.size()-PaddedZero; i++)
    {
        bit+=total[i];
        if(reversecodes.find(bit)!=reversecodes.end())
        {
            output+=reversecodes[bit];
            bit="";
        }
    }

    newfile.close();
    ofstream outputfile;
    outputfile.open("decompressed.txt",ios::out);
    outputfile << output;
    outputfile.close();

}

void CompressionRatio()
{

    int OldSize=0;
    for(int i=0; i<255; i++)
    {
        OldSize+=(8*ASCII[i]);
    }
    cout << "Old Size="<<OldSize<<endl;
    int NewSize=0;
    for(int i=0; i<255; i++)
    {
        NewSize+=(codes[i].length()*ASCII[i]);
    }
    cout << "New Size="<<NewSize<<endl;

    cout << "Compression Ration="<<((float)NewSize/OldSize)*100<<" %"<<endl;
}

void EncodeFolder(char filename[])
{
    fstream newfile;
    newfile.open(filename,fstream::in);
    char cr;

    while(newfile.get(cr))
    {
        totalFolder+=codes[cr];
        if (newfile.eof())
        {
            return;
        }
    }
    newfile.close();
    totalFolder+=codes[char(128)];
    newfile.close();
}

void WriteCompressed()
{

    ofstream outfile;
    vector <char> encoded;
    int zeros;
    if(totalFolder.size()%8!=0)
    {
        zeros=8-(totalFolder.size()%8);
        for(int j=0; j<zeros; j++)
        {
            totalFolder+="0";
        }
    }


    outfile.open("CompressedFolder.txt",ios::out);
    map<char, string>::iterator it;

    outfile << zeros;

    for (it = codes.begin(); it != codes.end(); it++)
    {
        outfile << it->first << char(236);
        outfile << it->second << char(153);
        outfile.flush();
    }
    outfile << char(234);
    stringstream sread(totalFolder);

    while(!sread.eof())
    {
        bitset<8>bit;
        sread>>bit;
        long dec = bit.to_ulong();
        encoded.push_back(char(dec));
        outfile.flush();
    }

    for(int k=0; k<encoded.size()-1; k++)
    {
        outfile << encoded.at(k);
    }

    outfile.close();

}



void BrowseFolder()
{
    char path[60];
    cout << "Enter path of folder"<<endl;
    cin >> path;
    char temppath[60];
    strcpy(temppath,path);
    DIR *dr;
    struct dirent *en;
    dr = opendir(path);
    vector <string> files;
    if (dr)
    {
        while ((en = readdir(dr)) != NULL)
        {
            string temptxt(en->d_name);
            if (temptxt.find(".txt") != std::string::npos)
            {
                strcpy(path,temppath);
                strcat(path,en->d_name);
                string temp(path);
                files.push_back(temp);
                ReadInputfile(path);
                ASCII[128]++;
            }
        }

        closedir(dr);
    }
    else
    {
        printf("INVALID DIRECTORY \n");
        exit(0);
    }
    BuildTree(ASCII);

    for(int i=0; i<files.size(); i++)
    {
        char char_array[files[i].length() + 1];
        strcpy(char_array, files[i].c_str());
        // printf("%s \n",char_array);
        EncodeFolder(char_array);

    }

    totalFolder.resize(totalFolder.size() - codes[char(128)].length());



    WriteCompressed();

}

void FolderDecompress()
{

    ifstream newfile;
    newfile.open("CompressedFolder.txt",ifstream::in);
    char paded0;
    newfile.get(paded0);
    char cr;
    char temp;
    newfile.get(cr);
    temp=cr;
    while(cr!=char(234))
    {
        string temp2="";
        if(cr==char(236))
        {
            newfile.get(cr);
            while(cr!=char(153))
            {
                codes[temp]+=cr;
                newfile.get(cr);
            }
        }
        else
        {
            temp=cr;
            newfile.get(cr);
        }
    }

    string total="";
    while(newfile.get(cr))
    {
        bitset<8> bit(cr);
        total+=bit.to_string();
    }
    string bit="",output="";
    map<string, char> reversecodes;

    for(map<char, string>::iterator i = codes.begin(); i!=codes.end(); i++)
    {
        reversecodes[i->second] = i->first;
    }

    int PaddedZero = paded0 - '0';
    string filename="output";
    int j=1;
    string ct="0";
    char tempp[60];
    cout << "Enter path for directory to be decompressed to:"<<endl;
    cin >> tempp;
    printf("\n");
    DIR *dr;
    struct dirent *en;
    dr = opendir(tempp);
    string path(tempp);
    if(dr)
    {
        for(int i=0; i<total.size()-PaddedZero; i++)
        {
            bit+=total[i];
            if(reversecodes.find(bit)!=reversecodes.end())
            {
                if(reversecodes[bit]!=char(128))
                {
                    output+=reversecodes[bit];
                    bit="";
                }
                else
                {
                    fstream file;
                    path+=filename;
                    ct=to_string(j);
                    path+=ct;
                    path+=".txt";
                    int n = path.length();
                    char char_array[n + 1];
                    strcpy(char_array, path.c_str());
                    // cout<<char_array<<endl;
                    file.open(char_array,ios::out);
                    file << output;
                    file.close();
                    j++;
                    output="";
                    bit="";
                    string t=".txt";
                    path.resize(path.size()-t.length());
                    path.resize(path.size()-ct.length());
                    path.resize(path.size()-filename.length());

                }


            }
        }
        fstream file;
        path+=filename;
        ct=to_string(j);
        path+=ct;
        path+=".txt";
        int n = path.length();
        char char_array[n + 1];
        strcpy(char_array, path.c_str());
        file.open(char_array,ios::out);
        file << output;
        file.close();
        j++;
        output="";
        string t=".txt";
        path.resize(path.size()-t.length());
        path.resize(path.size()-ct.length());

    }
    else
    {
        printf("INVALID DIRECTORY \n");
        exit(0);
    }

}



int case_insensitive_match(string s1, string s2)
{
    transform(s1.begin(), s1.end(), s1.begin(), ::tolower);
    transform(s2.begin(), s2.end(), s2.begin(), ::tolower);
    if(s1.compare(s2) == 0)
        return 1;
    return 0;
}

int main()

{

    string type;
    cout << "File or Folder \n";
    cin >> type;
    if(case_insensitive_match(type,"folder")==0 && case_insensitive_match(type,"file")==0)
    {
        printf("INVALID INPUT \n");
        return 0;
    }
    string choice;
    cout << "Compress or Decompress: \n";
    cin >> choice;
    if(case_insensitive_match(choice,"compress")==0 && case_insensitive_match(choice,"decompress")==0)
    {
        printf("INVALID INPUT \n");
        return 0;
    }
    if(case_insensitive_match(type,"file")==1)
    {
        Folder=0;
        if(case_insensitive_match(choice,"compress")==1)
        {
            char filename[20];
            cout << "Filename: ";
            cin >> filename;
            auto start = high_resolution_clock::now();
            ReadInputfile(filename);
            BuildTree(ASCII);
            compressFile(filename);
            auto stop = high_resolution_clock::now();
            CompressionRatio();
            auto duration = duration_cast<milliseconds>(stop - start);
            cout << "Compression time="<< duration.count() <<" milliseconds"<<endl;
        }
        else if(case_insensitive_match(choice,"decompress")==1)
        {
            auto start = high_resolution_clock::now();
            decompress();
            auto stop = high_resolution_clock::now();
            auto duration = duration_cast<milliseconds>(stop - start);
            cout << "decompression time="<< duration.count() << " milliseconds"<< endl;
        }


    }
    else if(case_insensitive_match(type,"folder")==1)
    {

        Folder=1;
        totalFolder="";
        if(case_insensitive_match(choice,"compress")==1)
        {
            auto start = high_resolution_clock::now();
            BrowseFolder();
            auto stop = high_resolution_clock::now();
            CompressionRatio();
            auto duration = duration_cast<milliseconds>(stop - start);
            cout << "Compression time="<< duration.count() <<" milliseconds"<<endl;




        }

        else if(case_insensitive_match(choice,"decompress")==1)
        {
            auto start = high_resolution_clock::now();
            FolderDecompress();
            auto stop = high_resolution_clock::now();
            auto duration = duration_cast<milliseconds>(stop - start);
            cout << "Decompression time="<< duration.count() <<" milliseconds"<<endl;

        }

    }

}
