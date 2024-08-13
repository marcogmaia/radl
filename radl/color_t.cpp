#include "color_t.hpp"
#include <array>
#include <cmath>

using std::fmod;
using std::max;
using std::min;

namespace radl {

namespace {

float clamp_float(float f) {
  if (f > 1.f) {
    f = 1.f;
  } else if (f < 0.f) {
    f = 0.f;
  }
  return f;
}

} // namespace

color_t::color_t(float R, float G, float B, float A)
    : r(static_cast<uint8_t>(255 * clamp_float(R))),
      g(static_cast<uint8_t>(255 * clamp_float(G))),
      b(static_cast<uint8_t>(255 * clamp_float(B))),
      a(static_cast<uint8_t>(255 * clamp_float(A))) {}

color_t::color_t(std::array<float, 4> &color) {
  r = static_cast<uint8_t>(255 * clamp_float(color[0]));
  g = static_cast<uint8_t>(255 * clamp_float(color[1]));
  b = static_cast<uint8_t>(255 * clamp_float(color[2]));
  a = static_cast<uint8_t>(255 * clamp_float(color[3]));
}

// Credit: https://gist.github.com/fairlight1337/4935ae72bcbcc1ba5c72
std::tuple<float, float, float> color_to_hsv(const color_t &col) {
  float fR = (col.r / 255.0f);
  float fG = (col.g / 255.0f);
  float fB = (col.b / 255.0f);

  float fH = 0.0f, fS = 0.0f, fV = 0.0f;

  float fCMax = max(max(fR, fG), fB);
  float fCMin = min(min(fR, fG), fB);
  float fDelta = fCMax - fCMin;

  if (fDelta > 0) {
    if (fCMax == fR) {
      fH = 60.0f * (fmodf(((fG - fB) / fDelta), 6.0f));
    } else if (fCMax == fG) {
      fH = 60.0f * (((fB - fR) / fDelta) + 2.0f);
    } else if (fCMax == fB) {
      fH = 60.0f * (((fR - fG) / fDelta) + 4.0f);
    }

    if (fCMax > 0) {
      fS = fDelta / fCMax;
    } else {
      fS = 0;
    }

    fV = fCMax;
  } else {
    fH = 0;
    fS = 0;
    fV = fCMax;
  }

  if (fH < 0) {
    fH = 360 + fH;
  }

  return std::make_tuple(fH, fS, fV);
}

// Credit: https://gist.github.com/fairlight1337/4935ae72bcbcc1ba5c72
std::tuple<uint8_t, uint8_t, uint8_t, uint8_t>
color_from_hsv(const float hue, const float saturation, const float value) {
  float fH = hue;
  float fS = saturation;
  float fV = value;

  float fR, fG, fB, fA;

  float fC = fV * fS; // Chroma
  float fHPrime = fmodf(fH / 60.0f, 6.0f);
  float fX = fC * (1.0f - fabsf(fmodf(fHPrime, 2.0f) - 1.0f));
  float fM = fV - fC;

  if (0 <= fHPrime && fHPrime < 1) {
    fR = fC;
    fG = fX;
    fB = 0;
  } else if (1 <= fHPrime && fHPrime < 2) {
    fR = fX;
    fG = fC;
    fB = 0;
  } else if (2 <= fHPrime && fHPrime < 3) {
    fR = 0;
    fG = fC;
    fB = fX;
  } else if (3 <= fHPrime && fHPrime < 4) {
    fR = 0;
    fG = fX;
    fB = fC;
  } else if (4 <= fHPrime && fHPrime < 5) {
    fR = fX;
    fG = 0;
    fB = fC;
  } else if (5 <= fHPrime && fHPrime < 6) {
    fR = fC;
    fG = 0;
    fB = fX;
  } else {
    fR = 0;
    fG = 0;
    fB = 0;
  }

  fR += fM;
  fG += fM;
  fB += fM;
  fA = 1.F;

  return std::make_tuple<uint8_t, uint8_t, uint8_t, uint8_t>(
      static_cast<uint8_t>(fR * 255.0), static_cast<uint8_t>(fG * 255.0),
      static_cast<uint8_t>(fB * 255.0), static_cast<uint8_t>(fA * 255.0));
}

/*
 * Calculates the luminance of a color, and converts it to grey-scale.
 */
color_t greyscale(const color_t &col) {
  unsigned char red = col.r;
  unsigned char green = col.g;
  unsigned char blue = col.b;

  float lred = red / 255.0F;
  float lgreen = green / 255.0F;
  float lblue = blue / 255.0F;
  float luminance = 0.299f * lred + 0.587f * lgreen + 0.114f * lblue;

  red = static_cast<unsigned char>(luminance * 255.0F);
  green = static_cast<unsigned char>(luminance * 255.0F);
  blue = static_cast<unsigned char>(luminance * 255.0F);

  return color_t(red, green, blue);
}

/*
 * Darkens a color by the specified amount.
 */
color_t darken(const int &amount, const color_t &col) {
  unsigned char red = col.r;
  unsigned char green = col.g;
  unsigned char blue = col.b;

  if (red > amount) {
    red -= amount;
  } else {
    red = 0;
  }
  if (green > amount) {
    green -= amount;
  } else {
    green = 0;
  }
  if (blue > amount) {
    blue -= amount;
  } else {
    blue = 0;
  }

  return color_t(red, green, blue);
}

color_t apply_colored_light(const color_t &col,
                            const std::tuple<float, float, float> &light) {
  unsigned char red = col.r;
  unsigned char green = col.g;
  unsigned char blue = col.b;

  float lred = red / 255.0F;
  float lgreen = green / 255.0F;
  float lblue = blue / 255.0F;

  lred *= std::get<0>(light);
  lgreen *= std::get<1>(light);
  lblue *= std::get<2>(light);

  if (lred > 1.0)
    lred = 1.0;
  if (lred < 0.0)
    lred = 0.0;
  if (lgreen > 1.0)
    lgreen = 1.0;
  if (lgreen < 0.0)
    lgreen = 0.0;
  if (lblue > 1.0)
    lblue = 1.0;
  if (lblue < 0.0)
    lblue = 0.0;

  red = static_cast<unsigned char>(lred * 255.0F);
  green = static_cast<unsigned char>(lgreen * 255.0F);
  blue = static_cast<unsigned char>(lblue * 255.0F);

  return color_t(red, green, blue);
}

color_t color_t::lerp(const color_t &first, const color_t &second,
                      float amount) {
  const auto &[r1, g1, b1, a1] = first;
  const auto &[r2, g2, b2, a2] = second;

  const float rdiff = r2 - r1;
  const float gdiff = g2 - g1;
  const float bdiff = b2 - b1;
  const float adiff = a2 - a1;

  float red = r1 + (rdiff * amount);
  float green = g1 + (gdiff * amount);
  float blue = b1 + (bdiff * amount);
  float alpha = a1 + (adiff * amount);

  static auto clamp_channel = [](float &channel) {
    if (channel < 0.f) {
      channel = 0.f;
    } else if (channel > 255.f) {
      channel = 255.f;
    }
  };

  clamp_channel(red);
  clamp_channel(green);
  clamp_channel(blue);
  clamp_channel(alpha);

  auto r = static_cast<const int>(red);
  auto g = static_cast<const int>(green);
  auto b = static_cast<const int>(blue);
  auto a = static_cast<const int>(alpha);

  return color_t{r, g, b, a};
}

} // namespace radl
