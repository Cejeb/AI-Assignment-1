#include "openai-helper.hpp"
#include "include/liboai.h"
#include <iostream>
#include <string>
#include <optional>
#include <vector>

// Basically, we need the OpenAI code in a separate translation unit than Raylib
// https://github.com/raysan5/raylib/issues/1217

namespace aipfg
{

openai_helper::openai_helper()
{
  oai_ = new liboai::OpenAI{};
}

bool openai_helper::init()
{
  if (!(*oai_).auth.SetKeyFile("../.env"))
  {
    std::cerr << "ERROR!: .env file not found.\n";
    return false;
  }

  return true;
}

void openai_helper::submit(const std::string& prompt, std::string& response_str,
                           const std::optional<std::vector<std::string>>& stop)
{
  const std::string model_id  = "text-davinci-003";
  // const std::optional<std::string> prompt = std::nullopt;
  const std::optional<std::string> suffix = std::nullopt;
  const std::optional<uint16_t> max_tokens = 150;
  const std::optional<float> temperature = 0.9f;
  const std::optional<float> top_p = std::nullopt;
  const std::optional<uint16_t> n = std::nullopt;
  const std::optional<std::function<bool(std::string, intptr_t)>> stream = std::nullopt;
  const std::optional<uint8_t> logprobs = std::nullopt;
  const std::optional<bool> echo = std::nullopt;
  //  const std::optional<std::vector<std::string>> stop = std::nullopt;
  const std::optional<float> presence_penalty = std::nullopt;
  const std::optional<float> frequency_penalty = std::nullopt;
  const std::optional<uint16_t> best_of = std::nullopt;
  const std::optional<std::unordered_map<std::string, int8_t>> logit_bias = std::nullopt;
  const std::optional<std::string> user = std::nullopt;

  liboai::Response response = (*oai_).Completion->create(model_id, prompt, suffix,
    max_tokens, temperature, top_p, n, stream, logprobs, echo, stop,
    presence_penalty, frequency_penalty, best_of, logit_bias, user);

  response_str = response["choices"][0]["text"].get<std::string>();
}

openai_helper::~openai_helper()
{
  delete oai_;
}

} // namespace aipfg
