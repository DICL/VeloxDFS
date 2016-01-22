%
% ``acmieeeSC'' LaTeX class.
% -
% - simplified version of Stephen Spencer's 'acmsiggraph' class.
% - version 0.1 2006/07/01 C.M.Pancake (pancake@nacse.org)
%
% ------------ identification --------------
\NeedsTeXFormat{LaTeX2e}
\ProvidesClass{acmieeeSC}[2005/01/22 ACM/IEEE SC Conferences]


% ------------ declaration of options --------------

\newcommand{\acm@pointsize}{}

\renewcommand{\acm@pointsize}{9pt}

\DeclareOption{twocolumn}{%
    \renewcommand{\acm@columnmode}{twocolumn}}
\DeclareOption{9pt}{%
    \renewcommand{\acm@pointsize}{9pt}}
\DeclareOption{10pt}{%
    \renewcommand{\acm@pointsize}{10pt}}
\DeclareOption{11pt}{%
    \renewcommand{\acm@pointsize}{11pt}}
\DeclareOption{12pt}{%
    \renewcommand{\acm@pointsize}{12pt}}

% Pass everything else to the 'artlcle' class, upon which this is based.

\DeclareOption*{\PassOptionsToClass{\CurrentOption}{article}}
\ProcessOptions
\PassOptionsToClass{twocolumn}{article}
\ifdim\acm@pointsize>9pt
    \PassOptionsToClass{\acm@pointsize}{article}
\fi

% ------------ package loading --------------

\LoadClass{article}

% ------------ main code --------------

\setlength{\textheight}{9.250in}
\setlength{\textwidth}{7.000in}
\setlength{\topmargin}{-0.700in}
\setlength{\oddsidemargin}{-0.250in}
\setlength{\evensidemargin}{-0.250in}
\setlength{\columnsep}{2pc}
\setlength{\headheight}{0.200in}
\setlength{\headsep}{0.250in}
\setlength{\footskip}{0.500in}
\setlength{\parindent}{1em}
\flushbottom

%% citation and reference definitions.
% enclose citations in square brackets.
% no labels in bibliography.

\def\@biblabel#1{}

% set the first line of each reference flush left, and each succeeding
% line with a 1em indentation.

\newlength{\bibhang}
\setlength{\bibhang}{1em}

%%%\def\thebibliography#1{\par\footnotesize
\def\thebibliography#1{%
  \section*{%
    \refname\@mkboth{\sl\uppercase{\refname}}{\sl\uppercase{\refname}}}
  \list{\relax}{\setlength{\labelsep}{0em}
                \setlength{\itemindent}{-\bibhang}
                \setlength{\leftmargin}{\bibhang}}
  \def\newblock{\hskip .11em plus .33em minus .07em}
  \sloppy\clubpenalty4000\widowpenalty4000
  \sfcode`\.=1000\relax}

\def\@citex[#1]#2{\if@filesw\immediate\write\@auxout{\string\citation{#2}}\fi
  \def\@citea{}\@cite{\@for\@citeb:=#2\do
    {\@citea\def\@citea{; }\@ifundefined
      {b@\@citeb}{{\bf ?}\@warning
      {Citation '\@citeb' on page \thepage \space undefined}}%
{\csname b@\@citeb\endcsname}}}{#1}}

\let\@internalcite\cite
\def\cite{\def\citename##1{##1}\@internalcite}
\def\shortcite{\def\citename##1{}\@internalcite}

%%

\renewcommand{\ps@plain}%
    {%
    \renewcommand{\@oddhead}{}%
    \renewcommand{\@oddfoot}{}%

    \renewcommand{\@evenhead}{\@oddhead}%
    \renewcommand{\@evenfoot}{\@oddfoot}%
    }

\renewcommand{\ps@empty}%
    {%
    \renewcommand{\@oddhead}{}%
    \renewcommand{\@oddfoot}{}%

    \renewcommand{\@evenhead}{\@oddhead}%
    \renewcommand{\@evenfoot}{\@oddfoot}%
    }

% no page numbers - they are added in production

\pagestyle{plain}

\newcommand{\acm@sectionfont}{}
\newcommand{\sectionfont}[1]{\renewcommand{\acm@sectionfont}{#1}}
\renewcommand\section{\@startsection {section}{1}{\z@}%
                {-3.5ex \@plus -1ex \@minus -.2ex}%
                {2.3ex \@plus.2ex}%
                {\reset@font\Large\sffamily\bfseries\acm@sectionfont}}
\renewcommand\subsection{\@startsection{subsection}{2}{\z@}%
                {-3.25ex\@plus -1ex \@minus -.2ex}%
                {1.5ex \@plus .2ex}%
                {\reset@font\large\sffamily\bfseries\acm@sectionfont}}
\renewcommand\subsubsection{\@startsection{subsubsection}{3}{\z@}%
                {-3.25ex\@plus -1ex \@minus -.2ex}%
                {1.5ex \@plus .2ex}%
                {\reset@font\normalsize\sffamily\bfseries\acm@sectionfont}}
\renewcommand\paragraph{\@startsection{paragraph}{4}{\z@}%
                {3.25ex \@plus1ex \@minus.2ex}%
                {-1em}%
                {\reset@font\normalsize\sffamily\bfseries\acm@sectionfont}}
\renewcommand\subparagraph{\@startsection{subparagraph}{5}{\parindent}%
                {3.25ex \@plus1ex \@minus .2ex}%
                {-1em}%
                {\reset@font\normalsize\sffamily\bfseries\acm@sectionfont}}

\newcommand{\acm@empty}{}

\newcommand{\acm@affiliation}{}
\newcommand{\affiliation}[1]{\renewcommand{\acm@affiliation}{#1}}

\newcommand{\acm@format}{}
\newcommand{\format}[1]{\renewcommand{\acm@format}{#1}}
\newcommand{\acmformat}[1]{\format{#1}}

\newlength{\titlespace}
\setlength{\titlespace}{0.25in}

    \renewcommand{\@maketitle}{%
        \begin{center}%
            {\LARGE\sffamily\bfseries\acm@sectionfont \@title \par}%
            \vspace{1\baselineskip}%
            \large
            \begin{tabular}[t]{c}%
                \@author
            \end{tabular}\par%
            \ifx\acm@affiliation\acm@empty \else%
                \vspace{0.5\baselineskip}%
                \acm@affiliation\par%
            \fi%
            \vspace{\titlespace}%
        \end{center} \par%
    }

\let\acm@origmaketitle\maketitle
\let\acm@origand\and
\renewcommand{\maketitle}{%
    \let\acm@title\@title%
    \let\acm@author\@author%
    \acm@origmaketitle%
    \thispagestyle{empty}%
}

\newcommand{\keywordlist}{%
    \vspace{0.5\baselineskip}%
    \par\noindent \textbf{Keywords: } \acm@keywords%
}

% copyright notice at the bottom of the left column on the first page

\newcommand{\acm@copyrightstrip}{}
\newcommand{\copyrightstrip}[1]{\renewcommand{\acm@copyrightstrip}{#1}}

\newcommand{\copyrightnotice}{%
    \renewcommand{\thefootnote}{}%
    \footnotetext[0]{%
\par\noindent
Permission to make digital or hard copies of all or part of this work 
for personal or classroom use is granted without fee provided that 
copies are not made or distributed for profit or commercial advantage 
and that copies bear this notice and the full citation on the first 
page. To copy otherwise, to republish, to post on servers or to 
redistribute to lists, requires prior specific permission and/or a fee.}%
    \footnotetext[0]{SC2006 November 2006, Tampa, Florida, USA}%
    \footnotetext[0]{\acm@copyrightstrip}%
    \renewcommand{\thefootnote}{\arabic{footnote}}
}

\renewcommand{\footnoterule}{%
    \kern-3pt
    \hrule width 0.45\columnwidth
    \kern 2.6pt}

\newcommand{\acm@contactname}{}
\newcommand{\contactname}[1]{\renewcommand{\acm@contactname}{#1}}

\newcommand{\acm@contactaddress}{}
\newcommand{\contactaddress}[1]{\renewcommand{\acm@contactaddress}{#1}}

\newcommand{\acm@contactphone}{}
\newcommand{\contactphone}[1]{\renewcommand{\acm@contactphone}{#1}}

\newcommand{\acm@contactfax}{}
\newcommand{\contactfax}[1]{\renewcommand{\acm@contactfax}{#1}}

\newcommand{\acm@contactemail}{}
\newcommand{\contactemail}[1]{\renewcommand{\acm@contactemail}{#1}}

\newcommand{\acm@keywords}{}
\newcommand{\keywords}[1]{\renewcommand{\acm@keywords}{#1}}

\InputIfFileExists{siggraph.cfg}
        {\typeout{***************************************^^J%
                  * Local config file siggraph.cfg used *^^J%
                  ***************************************}}
        {}
\endinput

% End of file 'acmieeeSC.cls'.

