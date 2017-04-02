/* bcdl - Bandcamp Download
 * Small program to download mp3s off bandcamp
 * Downloads directly into current directory, does not extract metadata.
 * 
 * Compiling requires GNU C++ with C++14 support:
 * g++-5 -std=c++14 -o bcdl bcdl.cpp
 *
 * Running requires wget is installed:
 * ./bcdl <url to album or track>
 *
 * Written on April 1, 2017 by Mark Swoope
 */
#include <iostream>
#include <regex>
#include <cstdlib>
#include <stdio.h>
#include <ext/stdio_filebuf.h>
using namespace std;

string wget_string(string url)
{
	char c;
	string result;
	ifstream pfs;
	ostringstream cmd;

	cmd << "wget -O - \"" << url << "\"";
	__gnu_cxx::stdio_filebuf<char> pfb(popen(cmd.str().c_str(), "r"), ios::in);
	pfs.rdbuf()->swap(pfb);
	while (pfs.get(c)) result += c;
	return result;	
}

int wget_file(string url, string path)
{
	ostringstream cmd;
	cmd << "wget -O \"" << path << "\" \"" << url << "\"";
	return system(cmd.str().c_str());
}

int rip(string webpage_data)
{
	regex track_re("\"title\":\"([^\"]*)\",[^{]*[{]\"mp3-128\":\"([^\"]*)\"[}]", regex::extended);
	smatch m;
	string title, url;
	int ec;

	while (regex_search(webpage_data, m, track_re)) {
		title = m.str(1);
		url = "http:"s + m.str(2);
		ec = wget_file(url, title + ".mp3"s);
		if (ec) return ec;
		webpage_data = m.suffix().str();
	}
	return 0;
}

int main(int argc, char *argv[])
{
	string url, webpage_data;

	if (argc != 2) {
		cout <<
		"Bandcamp ripper" << endl <<
		"Usage: bcdl URL" << endl <<
		"URL can be a track or album hosted on bandcamp.com" << endl;
		return 0;
	}
	return rip(wget_string(argv[1]));
}
