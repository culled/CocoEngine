//
// Created by cullen on 3/8/26.
//

#include "Color.h"

#include <cmath>

namespace Coco
{
    const Color Color::Black =		Color(0.0f, 0.0f, 0.0f, 1.0f);
    const Color Color::MidGrey =	Color(0.5f, 0.5f, 0.5f, 1.0f);
    const Color Color::White =		Color(1.0f, 1.0f, 1.0f, 1.0f);
    const Color Color::Red =		Color(1.0f, 0.0f, 0.0f, 1.0f);
    const Color Color::Green =		Color(0.0f, 1.0f, 0.0f, 1.0f);
    const Color Color::Blue =		Color(0.0f, 0.0f, 1.0f, 1.0f);
    const Color Color::Yellow =		Color(1.0f, 1.0f, 0.0f, 1.0f);
    const Color Color::Magenta =	Color(1.0f, 0.0f, 1.0f, 1.0f);
    const Color Color::Cyan =		Color(0.0f, 1.0f, 1.0f, 1.0f);
    const Color Color::ClearBlack = Color(0.0f, 0.0f, 0.0f, 0.0f);
    const Color Color::ClearWhite = Color(1.0f, 1.0f, 1.0f, 0.0f);

    Color::Color(float r, float g, float b, float a, bool isGamma) noexcept :
        Raw{r, g, b, a},
        IsGamma(isGamma)
    {}

    Color::Color(const Vector4& color, bool isGamma) :
        Color(color.X(), color.Y(), color.Z(), color.W(), isGamma)
    {}

    Color Color::AsLinear(float gamma) const noexcept
    {
        Color c(*this);
        c.ConvertToLinear(gamma);
        return c;
    }

    Color Color::AsGamma(float gamma) const noexcept
    {
        Color c(*this);
        c.ConvertToGamma(gamma);
        return c;
    }

    Color Color::WithAlpha(float alpha) const noexcept
    {
        return {R(), G(), B(), alpha, IsGamma};
    }

    void Color::ConvertToLinear(float gamma) noexcept
    {
        if (!IsGamma) return;

        R() = Math::Pow(R(), gamma);
        G() = Math::Pow(G(), gamma);
        B() = Math::Pow(B(), gamma);

        IsGamma = false;
    }

    void Color::ConvertToGamma(float gamma) noexcept
    {
        if (IsGamma) return;

        R() = Math::Pow(R(), 1.0f / gamma);
        G() = Math::Pow(G(), 1.0f / gamma);
        B() = Math::Pow(B(), 1.0f / gamma);

        IsGamma = true;
    }

    Vector4 Color::AsVector4(bool gammaCorrected) const noexcept
    {
        if (gammaCorrected == IsGamma)
            return {R(), G(), B(), A()};

        Color c = gammaCorrected ? AsGamma() : AsLinear();
        return {c.R(), c.G(), c.B(), c.A()};
    }
} // Coco