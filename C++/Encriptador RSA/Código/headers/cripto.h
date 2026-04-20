/* cripto.h */
#include <string>
#include <vector>
#include <string>
#ifndef CRIPTO_H // Include guard
#define CRIPTO_H
bool testaChaves(unsigned long long int &, unsigned long long int &, unsigned long long int &);
std::string getChaves(unsigned long long int &, unsigned long long int &, unsigned long long int &);
std::string codificacao(const std::string &, unsigned long long int &, unsigned long long int &);
std::string decodificacao(const std::string &, unsigned long long int &, unsigned long long int &);
#endif
