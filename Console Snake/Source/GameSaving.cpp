#include "GameSaving.h"
#include "LocalizedStrings.h"
#include "Resource.h"
#include "ScopeGuard.h"
#include <algorithm>
#include <fstream>
#include <filesystem>
#include <stdexcept>
#include <cryptopp/cryptlib.h>
#include <cryptopp/aes.h>
#include <cryptopp/modes.h>
#include <cryptopp/files.h>

// Helper Function
namespace {
	template<typename SrcType>
	class Convert // convert to any possible type
	{
	public:
		constexpr Convert(const SrcType& src_) noexcept :src(src_) {}
		Convert(const Convert&) = delete;
		Convert& operator=(const Convert&) = delete;

		template<typename BinType>
		constexpr operator BinType() const&& noexcept
		{
			return static_cast<BinType>(src);
		}
	private:
		const SrcType& src;
	};

	template<size_t N>
	std::string AES_encrypt(const unsigned char(&source)[N])
	{
		using namespace CryptoPP;

		static SecByteBlock key(crypto_key, AES::DEFAULT_KEYLENGTH);
		static SecByteBlock iv(crypto_IV, AES::BLOCKSIZE);
		CBC_Mode<AES>::Encryption encry;
		encry.SetKeyWithIV(key, key.size(), iv);

		std::string result;
		StringSource src(source, N, true,
			new StreamTransformationFilter(encry,
				new StringSink(result)
			)
		);
		return result;
	}

	std::string AES_decrypt(const unsigned char* source, size_t N)
	{
		using namespace CryptoPP;

		static SecByteBlock key(crypto_key, AES::DEFAULT_KEYLENGTH);
		static SecByteBlock iv(crypto_IV, AES::BLOCKSIZE);
		CBC_Mode<AES>::Decryption decry;
		decry.SetKeyWithIV(key, key.size(), iv);

		std::string result;
		StringSource src(source, N, true,
			new StreamTransformationFilter(decry,
				new StringSink(result)
			)
		);
		return result;
	}
}

// Constructor: read save file data to memory while program initializing
GameSavingBase::GameSavingBase() try
{
	using namespace std::filesystem;
	std::ios::sync_with_stdio(false);

	// Try to open the save file
	path save_file_path(save_file_name);
	std::ifstream save_file(save_file_path, std::ios::binary);
	if (!save_file.is_open())
		return;
	save_file.exceptions(std::ios_base::badbit | std::ios_base::failbit);

	// Read the encrypted data from save file 
	auto save_file_size = static_cast<size_t>(file_size(save_file_path));
	auto* cipher = new unsigned char[save_file_size] {};
	ON_SCOPE_EXIT{ delete[] cipher; };
	save_file.read(reinterpret_cast<char*>(cipher), save_file_size);

	// AES: 128-bit, CBC mode, PKCS7 padding
	std::string binary_pool;
	try {
		binary_pool = AES_decrypt(cipher, save_file_size);
	}
	catch (const CryptoPP::InvalidCiphertext&) {
		// The save file is invalid
		return;
	}

	// Store the decrypted data to bin_data and wait to be loaded
	if (sizeof bin_data != binary_pool.length())
		return; // The save file is invalid
	std::copy_n(binary_pool.c_str(), sizeof bin_data, reinterpret_cast<unsigned char*>(&bin_data));

	no_save_file = false;
}
catch (...)
{
	print_err(~token::message_init_gamesaving_fail);
	throw;
}

// convert fixed width binary data To game data
void GameSavingBase::convertFromBinaryData() noexcept
{
	if (no_save_file)
		return;
	// setting data
	{
		auto& gs = GameSetting::get();

		decltype(gs.theme.Value()) theme_temp;
		static_assert(sizeof theme_temp == sizeof bin_data.setting.theme, "abnormal struct align.");
		std::memcpy(&theme_temp, &bin_data.setting.theme, sizeof theme_temp);
		gs.theme.convertFrom(theme_temp);

		gs.speed.convertFrom(bin_data.setting.speed);
		gs.width.convertFrom(bin_data.setting.width);
		gs.height.convertFrom(bin_data.setting.height);
		LocalizedStrings::setLang(
			gs.lang.convertFrom(Convert{ bin_data.setting.lang })
		);
		gs.show_frame = Convert{ bin_data.setting.show_frame };
	}
	// rank data
	wchar_t buffer[Rank::name_max_length + 1] = {};
	for (int i = 0; i < Rank::rank_count; i++)
	{
		auto& save_item = bin_data.rank_list[i];
		auto& rank_item = Rank::get().getRank()[i];
		rank_item.score = Convert{ save_item.score };
		rank_item.width = Convert{ save_item.width };
		rank_item.height = Convert{ save_item.height };
		rank_item.speed = Convert{ save_item.speed };
		rank_item.is_win = Convert{ save_item.is_win };
		std::copy_n(save_item.name, Rank::name_max_length, buffer);
		rank_item.name = buffer;

		if (rank_item.is_win)
			GameData::get().colorful_title = true;
	}
}

// convert game data To fixed width binary data For saving
void GameSavingBase::convertToBinaryData() noexcept
{
	// setting data
	{
		auto& gs = GameSetting::get();

		auto theme_temp = gs.theme.Value();
		static_assert(sizeof theme_temp == sizeof bin_data.setting.theme, "abnormal struct align.");
		std::memcpy(&bin_data.setting.theme, &theme_temp, sizeof theme_temp);

		bin_data.setting.speed = Convert{ gs.speed.Value() };
		bin_data.setting.width = Convert{ gs.width.Value() };
		bin_data.setting.height = Convert{ gs.height.Value() };
		bin_data.setting.lang = Convert{ gs.lang.Value() };
		bin_data.setting.show_frame = Convert{ gs.show_frame };
	}
	// rank data
	Rank::get().lock();
	for (int i = 0; i < Rank::rank_count; i++)
	{
		auto& save_item = bin_data.rank_list[i];
		auto& rank_item = Rank::get().getRank_NoLock()[i];
		save_item.score = Convert{ rank_item.score };
		save_item.width = Convert{ rank_item.width };
		save_item.height = Convert{ rank_item.height };
		save_item.speed = Convert{ rank_item.speed };
		save_item.is_win = Convert{ rank_item.is_win };
		std::copy_n(rank_item.name.c_str(), Rank::name_max_length, save_item.name);
	}
	Rank::get().unlock();
}

// write save file from memory
void GameSavingBase::save()
{
	if (done.valid()) // wait for last time saving
		done.get();

	convertToBinaryData();

	done = std::async(std::launch::async,
					  [this]() noexcept
					  {
						  // Dump bin_data to binary_pool for processing
						  static unsigned char binary_pool[sizeof(SettingSavingItem) + sizeof(RankSavingItem) * Rank::rank_count] = {};
						  std::copy_n(reinterpret_cast<unsigned char*>(&bin_data), sizeof binary_pool, binary_pool);

						  try {
							  // AES: 128-bit, CBC mode, PKCS7 padding
							  std::string cipher;
							  cipher = AES_encrypt(binary_pool);

							  // Open the save file and write
							  std::ofstream save_file(save_file_name, std::ios::binary);
							  save_file.exceptions(std::ios_base::badbit | std::ios_base::failbit);
							  save_file.write(cipher.c_str(), cipher.size());
						  }
						  catch (const std::bad_alloc&) {
							  print_err(~token::message_std_bad_alloc);
							  system("pause");
							  exit(EXIT_FAILURE);
						  }
						  catch (const CryptoPP::Exception&) {
							  print_err(~token::message_process_savedata_fail);
							  print_err(~token::message_savefile_not_updated);
							  system("pause");
							  return;
						  }
						  catch (const std::exception&) {
							  print_err(~token::message_update_savefile_fail);
							  system("pause");
							  return;
						  }
						  catch (...) {
							  print_err(~token::message_unknown_error);
							  print_err(~token::message_savefile_not_updated);
							  system("pause");
							  exit(EXIT_FAILURE);
						  }
					  });
}