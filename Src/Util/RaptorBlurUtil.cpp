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

#include "RaptorBlurUtil.h"

QPixmap RaptorBlurUtil::invokeShadowGenerate(const QSize &qSize,
                                             const quint16 &qMargin,
                                             const quint16 &qRadius,
                                             const QColor &qColor)
{
    auto qImage = QImage(qSize + QSize(qMargin * 2, qMargin * 2), QImage::Format_ARGB32);
    const auto qRect = QRect(QPoint(qMargin, qMargin), qSize);
    qImage.fill(Qt::transparent);
    auto qPainter = QPainter(&qImage);
    qPainter.fillRect(qRect, qColor);
    qPainter.end();
    invokeExponentialBlur(12, 10, qImage, qRadius);

    auto qPaintes = QPainter{&qImage};
    qPaintes.setCompositionMode(QPainter::CompositionMode_SourceIn);
    qPaintes.fillRect(qImage.rect(), qColor);
    qPaintes.fillRect(QRect(qRect.topLeft(), qSize), Qt::transparent);
    qPaintes.setCompositionMode(QPainter::CompositionMode_SourceOver);
    qPaintes.end();
    return QPixmap::fromImage(qImage);
}

void RaptorBlurUtil::invokeExponentialBlur(const int &aprec,
                                           const int &zprec,
                                           QImage &qImage,
                                           const quint16 &qRadius)
{
    if (qRadius < 1)
    {
        return;
    }

    qImage = qImage.convertToFormat(QImage::Format_ARGB32_Premultiplied);

    /* Calculate the alpha such that 90% of
       the kernel is within the radius.
       (Kernel extends to infinity)
    */
    int alpha = (int) ((1 << aprec) * (1.0f - std::exp(-2.3f / (qRadius + 1.f))));

    int height = qImage.height();
    int width = qImage.width();
    for (int row = 0; row < height; row++)
    {
        invokeRowBlur(aprec, zprec, qImage, row, alpha);
    }

    for (int col = 0; col < width; col++)
    {
        invokeColumnBlur(aprec, zprec, qImage, col, alpha);
    }
}

void RaptorBlurUtil::invokeInnerBlur(const int &aprec,
                                     const int &zprec,
                                     unsigned char *bptr,
                                     int &zR,
                                     int &zG,
                                     int &zB,
                                     int &zA,
                                     int alpha)
{
    int R, G, B, A;
    R = *bptr;
    G = *(bptr + 1);
    B = *(bptr + 2);
    A = *(bptr + 3);

    zR += (alpha * ((R << zprec) - zR)) >> aprec;
    zG += (alpha * ((G << zprec) - zG)) >> aprec;
    zB += (alpha * ((B << zprec) - zB)) >> aprec;
    zA += (alpha * ((A << zprec) - zA)) >> aprec;

    *bptr = zR >> zprec;
    *(bptr + 1) = zG >> zprec;
    *(bptr + 2) = zB >> zprec;
    *(bptr + 3) = zA >> zprec;
}

void RaptorBlurUtil::invokeRowBlur(const int &aprec,
                                   const int &zprec,
                                   QImage &qImage,
                                   const int &qLine,
                                   const int &qAlpha)
{
    int zR, zG, zB, zA;

    QRgb *ptr = (QRgb *) qImage.scanLine(qLine);
    int width = qImage.width();

    zR = *((unsigned char *) ptr) << zprec;
    zG = *((unsigned char *) ptr + 1) << zprec;
    zB = *((unsigned char *) ptr + 2) << zprec;
    zA = *((unsigned char *) ptr + 3) << zprec;

    for (int index = 1; index < width; index++)
    {
        invokeInnerBlur(aprec, zprec, (unsigned char *) &ptr[index], zR, zG, zB, zA, qAlpha);
    }
    for (int index = width - 2; index >= 0; index--)
    {
        invokeInnerBlur(aprec, zprec, (unsigned char *) &ptr[index], zR, zG, zB, zA, qAlpha);
    }
}

void RaptorBlurUtil::invokeColumnBlur(const int &aprec,
                                      const int &zprec,
                                      QImage &qImage,
                                      const int &qColumn,
                                      const int &qAlpha)
{
    int zR, zG, zB, zA;

    QRgb *ptr = (QRgb *) qImage.bits();
    ptr += qColumn;
    int height = qImage.height();
    int width = qImage.width();

    zR = *((unsigned char *) ptr) << zprec;
    zG = *((unsigned char *) ptr + 1) << zprec;
    zB = *((unsigned char *) ptr + 2) << zprec;
    zA = *((unsigned char *) ptr + 3) << zprec;

    for (int index = width; index < (height - 1) * width; index += width)
    {
        invokeInnerBlur(aprec, zprec, (unsigned char *) &ptr[index], zR, zG, zB, zA, qAlpha);
    }

    for (int index = (height - 2) * width; index >= 0; index -= width)
    {
        invokeInnerBlur(aprec, zprec, (unsigned char *) &ptr[index], zR, zG, zB, zA, qAlpha);
    }
}
