//This file has been taken from KILE, merged together with their outputfilter

/***********************************************************************************
    begin                : Die Sep 16 2003
    copyright            : (C) 2003 by Jeroen Wijnhout (wijnhout@science.uva.nl)
 ***********************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef LATEXOUTPUTFILTER_H
#define LATEXOUTPUTFILTER_H

#include "mostQtHeaders.h"

enum LogType {LT_NONE = 0, LT_ERROR=1, LT_WARNING=2, LT_BADBOX=3, LT_MAX};
struct LatexLogEntry {
	static QColor textColors[LT_MAX];
	static QColor textColor(LogType lt) {return textColors[lt];}
	QString file;
	LogType type;
	//QString oldline;
	int oldline;
	int logline;
	QString message;
	LatexLogEntry();
	LatexLogEntry(QString aFile, LogType aType, int aOldline, int aLogline, QString aMessage);
	QString niceMessage(bool richFormat = true) const;
	void clear();
};

#define DEBUG_FILE_STACK 1
#if DEBUG_FILE_STACK
	#define PRINT_FILE_STACK(operation, file) {qDebug() << operation << file;}
#else
	#define PRINT_FILE_STACK(operation, file)
#endif

/**
 * An object of this class is used to parse the output messages
 * generated by a TeX/LaTeX-compiler.

 * @author Thorsten Lck
 * @author Jeroen Wijnhout
 */

class KTextEdit;

class LOFStackItem
{
public:
	explicit LOFStackItem(const QString& file = QString(), bool sure = false) : m_file(file), m_reliable(sure) {}

	const QString & file() const { return m_file; }
	void setFile(const QString & file) { m_file = file; }

	bool reliable() const { return m_reliable; }
	void setReliable(bool sure) { m_reliable = sure; }

private:
	QString m_file;
	bool m_reliable;
};

class OutputFilter : public QObject
{
    Q_OBJECT

public:
    OutputFilter();
    virtual ~OutputFilter();

protected:

public:
    //virtual bool Run(const QString& logfile);
	virtual bool run(const QTextDocument* log);

    //void setLog(const QString &log) { m_log = log; }
    const QString& log() const { return m_log; }

    void setSource(const QString &src);
    const QString& source() const  { return m_source; }
    const QString& path() const { return m_srcPath; }

protected:
    virtual short parseLine(const QString& strLine, short dwCookie);
    virtual bool OnTerminate();
    /**
    Returns the zero based index of the currently parsed line in the
    output file.
    */
    int GetCurrentOutputLine() const;

private:
    /** Number of current line in output file */
    unsigned int		m_nOutputLines;
    QString		m_log, m_source, m_srcPath;
};

class LatexOutputFilter : public OutputFilter
{
	friend class LatexOutputFilterTest;

    public:
        LatexOutputFilter();
        ~LatexOutputFilter();

	virtual bool run(const QTextDocument* log);
	//void sendProblems();
	//void updateInfoLists(const QString &texfilename, int selrow, int docrow);

	enum {Start = 0, FileName, HeuristicSearch, Error, Warning, BadBox, ExpectingBadBoxTextQoute, LineNumber,
		  // the following states are only used in updateFileStackHeuristic2
		  ExpectingFileName=10, InFileName, InQuotedFileName};

    protected:
        /**
        Parses the given line for the start of new files or the end of
        old files.
        */
        void updateFileStack(const QString &strLine, short & dwCookie);
		void updateFileStackHeuristic(const QString &strLine, short & dwCookie);
		void updateFileStackHeuristic2(const QString &strLine, short & dwCookie);
		static bool likelyNoFileStart(const QString &s, const QChar &nextChar);
		static bool fileNameLikelyComplete(const QString &s);
        /**
        Forwards the currently parsed item to the item list.
        */
        void flushCurrentItem();

        // overridings
    public:
        /** Return number of errors etc. found in log-file. */
	void getErrorCount(int *errors, int *warnings, int *badboxes);
	void clearErrorCount() { m_nErrors=m_nWarnings=m_nBadBoxes=0 ; }

    protected:
	virtual bool OnPreCreate();
	virtual short parseLine(const QString & strLine, short dwCookie);

	bool detectError(const QString & strLine, short &dwCookie);
	bool detectWarning(const QString & strLine, short &dwCookie);
	bool detectBadBox(const QString & strLine, short &dwCookie);
	bool detectLaTeXLineNumber(QString & warning, short & dwCookie, int len);
	bool detectBadBoxLineNumber(QString & strLine, short & dwCookie, int len);
	static bool isBadBoxTextQuote(const QString & strLine);

	bool fileExists(const QString & name);
	QString absoluteFileName(const QString & name); //returns "" if the file doesn't exists, uses m_filelookup
	
    public:
        /** number or errors detected */
        int m_nErrors;

        /** number of warning detected */
        int m_nWarnings;

        /** number of bad boxes detected */
        int m_nBadBoxes;

	int m_nParens;
	private:
        /**
        Stack containing the files parsed by the compiler. The top-most
        element is the currently parsed file.
        */	
        QStack<LOFStackItem> m_stackFile;

        /** The item currently parsed. */
        LatexLogEntry m_currentItem;

		QMap<QString, QString> m_filelookup; //maps relative filenames to absolute ones
    public:                                                                     // Public attributes
        /** Pointer to list of Latex output information */
        QList<LatexLogEntry> m_infoList;		
};
#endif
