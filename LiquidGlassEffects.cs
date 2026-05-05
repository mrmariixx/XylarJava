using Avalonia;
using Avalonia.Animation;
using Avalonia.Controls;
using Avalonia.Media;
using System;
using System.Collections.Generic;
using System.Threading.Tasks;

namespace XylarJavaLauncher;

public static class LiquidGlassEffects
{
    public static class PrimaryColors
    {
        public const string Highlight = "#F2EFE9";
        public const string MidTone = "#C9C4BC";
        public const string DeepTone = "#9A958E";
        public const string SoftLight = "#D8D4CC";
        public const string BluDark = "#1A1A1E";
        public const string BluMid = "#2A2A30";
        public const string BluDeep = "#1E1E22";
        public const string NavyBlack = "#121214";
    }

    public static void ApplyLiquidGlassStyle(Border border, bool withAnimation = true)
        => ApplyLiquidGlassStyle(border, new CornerRadius(28), withAnimation);

    public static void ApplySidebarLiquidGlass(Border border, bool withAnimation = true)
        => ApplyLiquidGlassStyle(border, new CornerRadius(18), withAnimation);

    public static void ApplyLiquidGlassStyle(Border border, CornerRadius cornerRadius, bool withAnimation = true)
    {
        if (border == null) return;

        var backgroundGradient = new LinearGradientBrush
        {
            StartPoint = new RelativePoint(0, 0, RelativeUnit.Relative),
            EndPoint = new RelativePoint(1, 1, RelativeUnit.Relative),
            GradientStops = new GradientStops
            {
                new GradientStop { Color = Color.Parse("#18FFFFFF"), Offset = 0 },
                new GradientStop { Color = Color.Parse("#2A2825AA"), Offset = 0.45 },
                new GradientStop { Color = Color.Parse("#0C0C0E99"), Offset = 1 }
            }
        };

        var borderGradient = new LinearGradientBrush
        {
            StartPoint = new RelativePoint(0, 0, RelativeUnit.Relative),
            EndPoint = new RelativePoint(1, 1, RelativeUnit.Relative),
            GradientStops = new GradientStops
            {
                new GradientStop { Color = Color.Parse(PrimaryColors.Highlight), Offset = 0 },
                new GradientStop { Color = Color.Parse(PrimaryColors.MidTone), Offset = 0.35 },
                new GradientStop { Color = Color.Parse(PrimaryColors.DeepTone), Offset = 0.55 },
                new GradientStop { Color = Color.Parse(PrimaryColors.MidTone), Offset = 0.75 },
                new GradientStop { Color = Color.Parse(PrimaryColors.Highlight), Offset = 1 }
            }
        };

        border.Background = backgroundGradient;
        border.BorderBrush = borderGradient;
        border.BorderThickness = new Thickness(1.5);
        border.CornerRadius = cornerRadius;
        border.Opacity = 1;

        if (withAnimation)
            AnimateBorderBreathe(border);
    }

    public static Border CreateShimmerOverlay(double width = 200, double height = 50)
    {
        var shimmer = new Border
        {
            Width = width,
            Height = height,
            CornerRadius = new CornerRadius(20),
            Opacity = 0.2,
            Background = new LinearGradientBrush
            {
                StartPoint = new RelativePoint(0, 0, RelativeUnit.Relative),
                EndPoint = new RelativePoint(1, 0, RelativeUnit.Relative),
                GradientStops = new GradientStops
                {
                    new GradientStop { Color = Colors.Transparent, Offset = 0 },
                    new GradientStop { Color = Colors.White, Offset = 0.5 },
                    new GradientStop { Color = Colors.Transparent, Offset = 1 }
                }
            }
        };
        return shimmer;
    }

    private static async void AnimateBorderBreathe(Border border)
    {
        if (border == null) return;

        var colors = new[]
        {
            PrimaryColors.Highlight,
            PrimaryColors.MidTone,
            PrimaryColors.DeepTone,
            PrimaryColors.SoftLight,
            PrimaryColors.MidTone
        };

        int colorIndex = 0;
        try
        {
            while (true)
            {
                await Task.Delay(2000);

                var nextColor = Color.Parse(colors[colorIndex % colors.Length]);
                var nextColorAlt = Color.Parse(colors[(colorIndex + 1) % colors.Length]);

                var animatedGradient = new LinearGradientBrush
                {
                    StartPoint = new RelativePoint(0, 0, RelativeUnit.Relative),
                    EndPoint = new RelativePoint(1, 0, RelativeUnit.Relative),
                    GradientStops = new GradientStops
                    {
                        new GradientStop { Color = nextColor, Offset = 0 },
                        new GradientStop { Color = nextColorAlt, Offset = 0.5 },
                        new GradientStop { Color = nextColor, Offset = 1 }
                    }
                };

                border.BorderBrush = animatedGradient;
                colorIndex++;
            }
        }
        catch (Exception ex)
        {
            System.Diagnostics.Debug.WriteLine($"Animation error: {ex.Message}");
        }
    }

    public static BoxShadow CreateGlowShadow(string color, double spread = 20)
    {
        var colorValue = Color.Parse(color);
        return new BoxShadow
        {
            Color = colorValue,
            Blur = spread,
            Spread = 0,
            OffsetX = 0,
            OffsetY = 0
        };
    }

    public static void ApplyGlassTextStyle(TextBlock textBlock, bool isBold = false)
    {
        if (textBlock == null) return;

        textBlock.Foreground = new SolidColorBrush(Color.Parse("#F6F8FF"));
        textBlock.FontSize = isBold ? 28 : 18;
        textBlock.FontWeight = isBold ? FontWeight.Bold : FontWeight.SemiBold;
    }

    public static void ApplyExtremeSidebarLiquidGlass(Border border)
    {
        if (border == null) return;

        var backgroundBrush = new LinearGradientBrush
        {
            StartPoint = new RelativePoint(0, 0, RelativeUnit.Relative),
            EndPoint = new RelativePoint(1, 1, RelativeUnit.Relative),
            GradientStops = new GradientStops
            {
                new GradientStop { Color = Color.Parse("#C2FFFFFF"), Offset = 0.0 },
                new GradientStop { Color = Color.Parse("#080808"), Offset = 0.20 },
                new GradientStop { Color = Color.Parse("#121212"), Offset = 0.50 },
                new GradientStop { Color = Color.Parse("#0A0A0A"), Offset = 0.80 },
                new GradientStop { Color = Color.Parse("#D9FFFFFF"), Offset = 1.0 }
            }
        };

        var borderBrush = new LinearGradientBrush
        {
            StartPoint = new RelativePoint(0, 0, RelativeUnit.Relative),
            EndPoint = new RelativePoint(1, 1, RelativeUnit.Relative),
            GradientStops = new GradientStops
            {
                new GradientStop { Color = Color.Parse("#FFFFFF"), Offset = 0.0 },
                new GradientStop { Color = Color.Parse("#A0A0A0"), Offset = 0.25 },
                new GradientStop { Color = Color.Parse("#404040"), Offset = 0.50 },
                new GradientStop { Color = Color.Parse("#B0B0B0"), Offset = 0.75 },
                new GradientStop { Color = Color.Parse("#FFFFFF"), Offset = 1.0 }
            }
        };

        border.Background = backgroundBrush;
        border.BorderBrush = borderBrush;
        border.BorderThickness = new Thickness(2.5);
        border.CornerRadius = new CornerRadius(32);
        border.Opacity = 0.76;

        AnimateExtremeBorder(border);
    }

    private static async void AnimateExtremeBorder(Border border)
    {
        if (border == null) return;
        try
        {
            var colors = new[] { "#FFFFFF", "#E0E0E0", "#C0C0C0", "#E0E0E0", "#FFFFFF" };
            var index = 0;
            while (true)
            {
                await Task.Delay(2000);
                var brush = new LinearGradientBrush
                {
                    StartPoint = new RelativePoint(0, 0, RelativeUnit.Relative),
                    EndPoint = new RelativePoint(1, 1, RelativeUnit.Relative),
                    GradientStops = new GradientStops
                    {
                        new GradientStop { Color = Color.Parse(colors[index % colors.Length]), Offset = 0.0 },
                        new GradientStop { Color = Color.Parse("#808080"), Offset = 0.25 },
                        new GradientStop { Color = Color.Parse("#404040"), Offset = 0.50 },
                        new GradientStop { Color = Color.Parse("#A0A0A0"), Offset = 0.75 },
                        new GradientStop { Color = Color.Parse(colors[(index + 1) % colors.Length]), Offset = 1.0 }
                    }
                };
                border.BorderBrush = brush;
                index++;
            }
        }
        catch { }
    }

    public static Button CreateLiquidGlassButton(string content, bool isPrimary = true)
    {
        var button = new Button
        {
            Content = content,
            Padding = new Thickness(20, 12),
            CornerRadius = new CornerRadius(14),
            FontWeight = isPrimary ? FontWeight.Bold : FontWeight.SemiBold
        };

        if (isPrimary)
        {
            button.Background = new SolidColorBrush(Color.Parse(PrimaryColors.MidTone));
            button.Foreground = new SolidColorBrush(Color.Parse("#07111D"));
        }
        else
        {
            button.Background = new SolidColorBrush(Color.Parse("#353540"));
            button.Foreground = new SolidColorBrush(Color.Parse("#F6F8FF"));
        }

        return button;
    }
}
