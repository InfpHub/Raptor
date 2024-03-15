/**
 *
 * Exponential blur, Jani Huhtanen, 2006
 *
 * expblur(QImage &img, int radius)
 *
 * In-place blur of image 'img' with kernel
 * of approximate radius 'radius'.
 *
 * Blurs with two sided exponential impulse
 * response.
 *
 * aprec = precision of alpha parameter
 * in fixed-point format 0.aprec
 *
 * zprec = precision of state parameters
 * zR,zG,zB and zA in fp format 8.zprec
 *
 */

#ifndef RAPTORBLURUTIL_H
#define RAPTORBLURUTIL_H

#include <QImage>
#include <QPainter>
#include <QPixmap>

#include <cmath>

class RaptorBlurUtil Q_DECL_FINAL
{
public:
    static QPixmap invokeShadowGenerate(const QSize &qSize,
                                        const quint16 &qMargin,
                                        const quint16 &qRadius,
                                        const QColor &qColor);

    static void invokeExponentialBlur(const int &aprec,
                                      const int &zprec,
                                      QImage &qImage,
                                      const quint16 &qRadius);

private:
    static void invokeInnerBlur(const int &aprec,
                                const int &zprec,
                                unsigned char *bptr,
                                int &zR,
                                int &zG,
                                int &zB,
                                int &zA,
                                int alpha);

    static void invokeRowBlur(const int &aprec,
                              const int &zprec,
                              QImage &qImage,
                              const int &qLine,
                              const int &qAlpha);

    static void invokeColumnBlur(const int &aprec,
                                 const int &zprec,
                                 QImage &qImage,
                                 const int &qColumn,
                                 const int &qAlpha);
};

#endif // RAPTORBLURUTIL_H
