#include <iostream>

#include <QFile>
#include <QRect>
#include <QPaintEvent>
#include <QPainter>
#include <QFontMetrics>
#include <QScrollBar>

#include "log.h"

#include "common.h"
#include "logmainview.h"

LogMainView::LogMainView(QWidget* parent) : QAbstractScrollArea(parent)
{
    logData = NULL;
    // Create the viewport QWidget
    setViewport(0);
}

void LogMainView::resizeEvent(QResizeEvent* resizeEvent)
{
    if ( logData == NULL )
        return;

    LOG(logDEBUG) << "resizeEvent received";

    // Calculate the index of the last line shown
    lastLine = min2( logData->getNbLine(), firstLine + getNbVisibleLines() );

    // Update the scroll bars
    verticalScrollBar()->setPageStep( getNbVisibleLines() );
}

void LogMainView::scrollContentsBy( int dx, int dy )
{
    LOG(logDEBUG) << "scrollContentsBy received";

    firstLine -= dy;
    lastLine = min2( logData->getNbLine(), firstLine + getNbVisibleLines() );

    update();
}

void LogMainView::paintEvent(QPaintEvent* paintEvent)
{
    QRect invalidRect = paintEvent->rect();
    if ( (invalidRect.isEmpty()) || (logData == NULL) )
        return;

    LOG(logDEBUG) << "paintEvent received, firstLine=" << firstLine
        << " lastLine=" << lastLine;

    {
        // Repaint the viewport
        QPainter painter(viewport());
        int fontHeight = painter.fontMetrics().height();
        // painter.fillRect(invalidRect, Qt::white); // useless...
        painter.setPen( Qt::black );
        for (int i = firstLine; i < lastLine; i++) {
            int yPos = (i-firstLine + 1) * fontHeight;
            painter.drawText(0, yPos, logData->getLineString(i));
        }
    }

}

int LogMainView::getNbVisibleLines()
{
    QFontMetrics fm = fontMetrics();
    return height()/fm.height() + 1;
}

bool LogMainView::readFile(const QString &fileName)
{
    QFile file(fileName);

    if (file.open(QFile::ReadOnly | QFile::Text))
    {
        if (logData)
            delete logData;
        logData = new LogData(file.readAll());

        // Adapt the view to the new content
        LOG(logDEBUG) << "Now adapting the content";
        verticalScrollBar()->setValue( 0 );
        verticalScrollBar()->setRange( 0, logData->getNbLine()-1 );
        firstLine = 0;
        lastLine = min2( logData->getNbLine(), firstLine + getNbVisibleLines() );
        update();

        return true;
    }
    else
        return false;
}