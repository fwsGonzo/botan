// common code for the validation and benchmark code

#ifndef BOTAN_CHECK_COMMON_H__
#define BOTAN_CHECK_COMMON_H__

#include <vector>
#include <string>
#include <deque>
#include <stdexcept>

#include <botan/secmem.h>

using Botan::byte;
using Botan::u32bit;

struct algorithm
   {
      algorithm(const char* t, const char* n,
                u32bit k = 0, u32bit i = 0) :
         type(t), name(n), filtername(n), keylen(k), ivlen(i) {}
      algorithm(const char* t, const char* n,
                const char* f, u32bit k = 0, u32bit i = 0) :
         type(t), name(n), filtername(f), keylen(k), ivlen(i) {}
      std::string type, name, filtername;
      u32bit keylen, ivlen, weight;
   };

std::vector<algorithm> get_algos();

void strip_comments(std::string& line);
void strip_newlines(std::string& line);
void strip(std::string& line);
std::vector<std::string> parse(const std::string& line);

std::string hex_encode(const byte in[], u32bit len);
Botan::SecureVector<byte> decode_hex(const std::string&);

Botan::u64bit get_clock();
Botan::u64bit get_ticks();

class Fixed_Output_RNG : public Botan::RandomNumberGenerator
   {
   public:
      bool is_seeded() const { return !buf.empty(); }

      byte random()
         {
         if(buf.empty())
            throw std::runtime_error("Out of bytes");

         byte out = buf.front();
         buf.pop_front();
         return out;
         }

      void randomize(byte out[], u32bit len) throw()
         {
         for(u32bit j = 0; j != len; j++)
            out[j] = random();
         }

      std::string name() const { return "Fixed_Output_RNG"; }

      void clear() throw() {}

      void add_randomness(const byte in[], u32bit len) throw()
         {
         buf.insert(buf.end(), in, in + len);
         }

      Fixed_Output_RNG(const Botan::SecureVector<byte>& x)
         {
         add_randomness(x.begin(), x.size());
         }
      Fixed_Output_RNG(const std::string& in)
         {
         Botan::SecureVector<byte> x = decode_hex(in);
         add_randomness(x.begin(), x.size());
         }

      Fixed_Output_RNG() {}
   private:
      std::deque<byte> buf;
   };

#endif
