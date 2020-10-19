#pragma once

/*
int GetCommandLineProperty_Int(const int argc, char** argv, char* propertyName, const int default_return)
{
for (int i = 0; i < argc; ++i)
{
if (!_strcmpi(propertyName, argv[i]))
{
return atoi(argv[++i]);
}
}

return default_return;
}


float GetCommandLineProperty_Float(const int argc, char** argv, char* propertyName, const float default_return)
{
return GetCommandLineProperty_Int(argc, argv, propertyName, (default_return *100)) / 100.0f;
}


char* GetCommandLineProperty_String(const int argc, char** argv, char* propertyName, char* default_return)
{
for (int i = 0; i < argc; ++i)
{
if (!_strcmpi(propertyName, argv[i]))
{
return argv[++i];
}
}

return default_return;
}


/*
const char* GetProperty_String(const char* propertyName, const char* default_return, vector<KeyValPair>& configBlob)
{
for (KeyValPair kvp : configBlob)
{
if (_strcmpi(kvp.key.c_str(), propertyName) == 0)
{
return kvp.val.c_str();
}
}

return default_return;
}
*/