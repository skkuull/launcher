#include "http.hpp"
#include <curl/curl.h>
#include <gsl/gsl>

#pragma comment(lib, "ws2_32.lib")

namespace utils::http
{
	namespace
	{
		struct write_helper
		{
			const std::function<void(size_t)>* callback;
			std::string* data;
		};
	
		size_t write_callback(void* contents, const size_t size, const size_t nmemb, void* userp)
		{
			auto* helper = static_cast<write_helper*>(userp);

			const auto total_size = size * nmemb;
			helper->data->append(static_cast<char*>(contents), total_size);
			if (*helper->callback)
			{
				OutputDebugStringA(std::to_string(total_size).data());
				(*helper->callback)(helper->data->size());
			}
			return total_size;
		}
	}

	std::optional<std::string> get_data(const std::string& url, const headers& headers, const std::function<void(size_t)>& callback)
	{
		curl_slist* header_list = nullptr;
		auto* curl = curl_easy_init();
		if (!curl)
		{
			return {};
		}

		auto _ = gsl::finally([&]()
		{
			curl_slist_free_all(header_list);
			curl_easy_cleanup(curl);
		});

		
		for(const auto& header : headers)
		{
			auto data = header.first + ": " + header.second;
			header_list = curl_slist_append(header_list, data.data());
		}

		std::string buffer{};
		write_helper helper{};
		helper.data = &buffer;
		helper.callback = &callback;
		
		curl_easy_setopt(curl, CURLOPT_HTTPHEADER, header_list);
		curl_easy_setopt(curl, CURLOPT_URL, url.data());
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &helper);
		curl_easy_setopt(curl, CURLOPT_TIMEOUT, 1);
		if (curl_easy_perform(curl) == CURLE_OK)
		{
			return {std::move(buffer)};
		}

		return {};
	}

	std::future<std::optional<std::string>> get_data_async(const std::string& url, const headers& headers)
	{
		return std::async(std::launch::async, [url, headers]()
		{
			return get_data(url, headers);
		});
	}
}
