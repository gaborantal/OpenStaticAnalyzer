#include <fstream>
#include <iostream>
#include <lim/inc/lim.h>
#include <csi/inc/csi.h>
#include <ErrorCodes.h>
#include <common/inc/FileSup.h>
#include <ReleaseVersion.h>
#include <common/inc/Stat.h>
#include <common/inc/StringSup.h>
#include <rul/inc/RulHandler.h>
#include <common/inc/WriteMessage.h>
#include <common/inc/Arguments.h>
#include <limmetrics/inc/LimMetrics.h>

#include "../inc/LIM2Stink.h"

#include <graph/inc/graph.h>
#include <lim2graph/inc/Lim2GraphConverter.h>
#include <graphsupport/inc/CsvExporter.h>
#include <graphsupport/inc/MetricSum.h>
#include <graphsupport/inc/RulBuilder.h>

#include <io/inc/IO.h>

#define PROGRAM_NAME "LIM2Stink"
#define EXECUTABLE_NAME PROGRAM_NAME

#include "MainCommon.h"

using namespace std;
using namespace common;
using namespace columbus;
using namespace columbus::rul;
using namespace columbus::lim::asg;
using namespace columbus::lim::metrics;
using namespace columbus::lim::antipatterns;



// constants
const unsigned int UNREADABLE      = 2;
const char*        DEF_INI_FILE    = "Stink.rul";
const string       FLT_EXT         = ".lisi";
const string       UNDEFINED       = "~~@~~";
const char*        CSV_EXT         = ".csv";
const char*        LIMML_EXT       = ".limml";
const char*        MEM_FILE_EXT    = ".memstat";
const char*        UNAME_SEP       = "*";
const char*        COMP_SEP        = "|";

// parameters
static string listFile = UNDEFINED;
static string metric = UNDEFINED;
static string warnings = UNDEFINED;

static bool all = false;
static bool decl = false;
static string rulfile = DEF_INI_FILE;
static string home = UNDEFINED;
static string rulconfig = "";
static string separator = ";";
static bool nocomponent = false;

static string statFile = UNDEFINED;
static string memstatFile = UNDEFINED;

static bool noIds = false;
static string graph_filename;
static bool exportRul = false;

static list<string> inputFiles;

// Callback methods for argument processing
static bool ppList (const Option *o, char *argv[]) {
  listFile = argv[0];
  return true;
}

static bool ppMetrics (const Option *o, char *argv[]) {
  metric = argv[0];
  return true;
}

static bool ppWarnings (const Option *o, char *argv[]) {
  warnings = argv[0];
  return true;
}

static bool ppSeparator (const Option *o, char *argv[]) {
  separator = argv[0];
  return true;
}

static bool ppRul (const Option *o, char *argv[]) {
  rulfile = argv[0];
  return true;
}

static bool ppRulConfig (const Option *o, char *argv[]) {
  rulconfig = argv[0];
  return true;
}
static bool ppAll (const Option *o, char *argv[]) {
  all = true;
  return true;
}

static bool ppDecl (const Option *o, char *argv[]) {
  decl = true;
  return true;
}

static bool ppNoComp (const Option *o, char *argv[]) {
  nocomponent = true;
  return true;
}

static bool ppStat (const Option *o, char *argv[]) {
  statFile = argv[0];
  return true;
}

static bool ppMemstat (const Option *o, char *argv[]) {
  memstatFile = argv[0];
  return true;
}

static bool ppNoIDs (const Option *o, char *argv[]) {
  noIds = true;
  return true;
}

static bool ppGraph (const Option *o, char *argv[]) {
  graph_filename = argv[0];
  return true;
}

static void ppFile(char *filename) {
  inputFiles.push_back(filename);
}

static bool ppExportRul(const Option *o, char *argv[]) {
  exportRul = true;
  return true;
}

const Option OPTIONS_OBJ [] = {
  { false, "-list",         1,  "listFile",   0, OT_WC,    ppList,       NULL,   "The file containing the list of input files"},
  { false, "-metrics",      1,  "fileName",   0, OT_WC,    ppMetrics,    NULL,   "Output file for antipattern's metrics"},
  { false, "-warnings",     1,  "fileName",   0, OT_WC,    ppWarnings,   NULL,   "Output file for antipattern's monitor warnings"},
  { false, "-s",            1,  "character",  0, OT_WC,    ppSeparator,  NULL,   "Separator character"},
  { false, "-rul",          1,  "rulFile",    0, OT_WC,    ppRul,        NULL,   "The .rul file (default: Stink.rul)"},
  { false, "-rulconfig",    1,  "config",     0, OT_WC,    ppRulConfig,  NULL,   "The configuration of the .rul file"},
  { false, "-all",          0,  "",           0, OT_NONE,  ppAll,        NULL,   "All antipatterns defined for the given language are calculated"},
  { false, "-decl",         0,  "",           0, OT_NONE,  ppDecl,       NULL,   "Declarations and their metrics are written out"},
  { false, "-nocomponent",  0,  "",           0, OT_NONE,  ppNoComp,     NULL,   "Components are not written out (the column is empty)\n"},
  { false, "-stat",         1,  "statFile",   0, OT_WC,    ppStat,       NULL,   "Statistics (memory usage, time) is written into <file>"},
  { false, "-noIds",        0,  "",           0, OT_NONE,  ppNoIDs,      NULL,   "LIMML dump is written out without ids\n"},
  { false, "-graph",        1,  "graphFile",  0, OT_WC,    ppGraph,      NULL,   "Save binary graph output."},
  { false, "-exportRul",    0,  "",           0, OT_WC,    ppExportRul,  NULL,   "Save rul into graph output."},
  COMMON_CL_ARGS
};


void checkInputFiles() {

  if (listFile != UNDEFINED) {
    ifstream ifs(listFile.c_str());
    if (ifs.is_open()) {
      string tmp;
      while (getline(ifs, tmp)) {
        if (!tmp.length())
          continue;
        if (tmp[tmp.length()-1]==0xD) { // handle DOS line-ends in unix
          inputFiles.push_back(tmp.substr(0,tmp.length()-1));
        } else {
          inputFiles.push_back(tmp);
        }
      }
      ifs.close();
    }
  }

  if (inputFiles.size() == 0)
    clError();
}


void checkMemStatFile() {
  if (memstatFile == UNDEFINED)
    return;
    
  if (!memstatFile.length()) {
    memstatFile = common::pathRemoveExtension(metric) + MEM_FILE_EXT;
  }

  ofstream ofs(memstatFile.c_str());
  if (!ofs.is_open()) {
    WriteMsg::write(WriteMsg::mlError, "ERROR: Cannot open memory stat file: \"%s\" (statistics will not be created)\n", memstatFile.c_str());
    memstatFile = UNDEFINED;
    return;
  }
  ofs.close();  
}

void checkSeparator() {
  if (separator.length() == 3 && separator[0] == separator[2] && separator[0] == '"')
    separator = separator[1];
}

void writeMemStat(const string& msg) {
  if (memstatFile == UNDEFINED)
    return;
  ofstream ofs(memstatFile.c_str(),ofstream::app);
  if (!ofs.is_open()) {
    WriteMsg::write(WriteMsg::mlError, "ERROR: Cannot open file: %s\n", memstatFile.c_str());
    return;
  }
  ofs << msg << articulateNumber(common::getProcessUsedMemSize().size) << endl; 
  ofs.close();
}


void selectPatterns( RulHandler& rl, set<string> &patterns, int patternNum, const char*(*getName)(unsigned int)) {

  for(int i = 0; i < patternNum; i++) {
    if(rl.getIsDefined(getName(i))) {
      if(rl.getIsEnabled(getName(i))) {
        patterns.insert(getName(i));
        cout << "OK: " << getName(i) << endl;
      } else cout << "NOT ENABLED: " << getName(i) << endl;
    } else cout << "NOT DEFINED: " << getName(i) << endl;
  }

}


void WriteStats(unsigned int memory) {

  if (statFile == UNDEFINED)
    return;

  common::timestat usedtime = common::getProcessUsedTime();

  if (statFile == "") { // wtiting into output stream

    WriteMsg::write(WriteMsg::mlWarning, "%s%u%s%ul%s%ul", separator.c_str(), memory, separator.c_str(), usedtime.user, separator.c_str(), usedtime.system);

  } else {

    ofstream os;
    os.open(statFile.c_str(), ofstream::out | ofstream::app);
    if (!os.is_open()) {
      WriteMsg::write(WriteMsg::mlError, "ERROR: Cannot open file: %s\n", statFile.c_str());
      return;
    }

    os << separator << memory
       << separator << usedtime.user
       << separator << usedtime.system;

    os.close();

  }
}

// TODO refactor into common, copied from LIM2Metrics
void loadFilter(lim::asg::Factory& fact, const string& file) {
  string flt = common::pathRemoveExtension(file) + ".flim";
  if (common::fileTimeCmp(flt, file) == -1) {
    common::WriteMsg::write( common::WriteMsg::mlWarning, "%s is older than the .lim", flt.c_str());
    return;
  }

  try {
    fact.loadFilter(flt);
  } catch (IOException e) {
    common::WriteMsg::write( common::WriteMsg::mlWarning, "%s cannot be loaded", flt.c_str());
    return;
  }
}


// Copied from CsvExporter --> refactor graphsupport
graph::Node getParent( graph::Node& node )
{
  using namespace graph;

  Node parent = Graph::invalidNode;
  Edge::EdgeIterator edgeIt = node.findOutEdges( Edge::EdgeType( "LogicalTree", Edge::edtReverse ) );
  if ( edgeIt.hasNext() )
  {
    parent = edgeIt.next().getToNode();
  }
  return parent;
}

string getLongName( graph::Node& node )
{
  using namespace graph;

  graph::Attribute::AttributeIterator i = node.findAttributeByName( "LongName" );
  if ( i.hasNext() )
  {
    return static_cast<AttributeString&>(i.next()).getValue();
  }

  return "";
}

/*
void exportCsv( graph::Graph& graph, const string& filename )
{
  using namespace graph;
  using namespace io;

  try 
  {
    CsvIO csvOut( filename, IOBase::omWrite );
    csvOut.setSeparator( '|' );

    // write out column headers
    csvOut.writeColumn( CMSG_STAT_HEADER_UID );
    csvOut.writeColumn( CMSG_STAT_HEADER_NAME );
    csvOut.writeColumn( CMSG_STAT_HEADER_PARENTUID );
    csvOut.writeColumn( CMSG_STAT_HEADER_PARENTNAME );
    csvOut.writeColumn( CMSG_STAT_HEADER_KIND );
    csvOut.writeColumn( CMSG_STAT_HEADER_PATH );
    csvOut.writeColumn( CMSG_STAT_HEADER_LINE );
    csvOut.writeColumn( CMSG_STAT_HEADER_COL );
    csvOut.writeColumn( CMSG_STAT_HEADER_ENDLINE );
    csvOut.writeColumn( CMSG_STAT_HEADER_ENDCOL );
    csvOut.writeColumn( CMSG_STAT_HEADER_WARNINGID );
    csvOut.writeNewLine();

    // write out warnings
    Node::NodeIterator nodeIt = graph.getNodes();

    while( nodeIt.hasNext() )
    {
      Node node = nodeIt.next();
      string nodeName = getLongName( node );

      Node parent = getParent( node );
      string parentUID = "";
      string parentName = "";
      if ( parent != Graph::invalidNode )
      {
        parentUID = parent.getUID();
        parentName = getLongName( parent );
      }


      graph::Attribute::AttributeIterator attributeIt = node.findAttributeByContext( "warning" );
      
      while( attributeIt.hasNext() )
      {
        graph::AttributeComposite& attr = static_cast<AttributeComposite&>(attributeIt.next());
        csvOut.writeColumn( node.getUID() );
        csvOut.writeColumn( nodeName );
        csvOut.writeColumn( parentUID );
        csvOut.writeColumn( parentName );
        csvOut.writeColumn( node.getType().getType() );

        string path = "", warning = "";
        int line = 0, col = 0, endLine = 0, endCol = 0;
            
        warning = attr.getName();
            

        graph::Attribute::AttributeIterator warningAttrIt = attr.findAttributeByName("Path");
        if ( warningAttrIt.hasNext() ) path = static_cast<AttributeString&>(warningAttrIt.next()).getValue();

        warningAttrIt = attr.findAttributeByName("Line"); // TODO to graph::constants!!
        if ( warningAttrIt.hasNext() ) line = static_cast<AttributeInt&>(warningAttrIt.next()).getValue();

        warningAttrIt = attr.findAttributeByName("Column");
        if ( warningAttrIt.hasNext() ) col = static_cast<AttributeInt&>(warningAttrIt.next()).getValue();

        warningAttrIt = attr.findAttributeByName("EndLine");
        if(warningAttrIt.hasNext())
          endLine = static_cast<AttributeInt&>(warningAttrIt.next()).getValue();

        warningAttrIt = attr.findAttributeByName("EndColumn");
        if(warningAttrIt.hasNext())
          endCol = static_cast<AttributeInt&>(warningAttrIt.next()).getValue();

        csvOut.writeColumn(path);
        csvOut.writeColumn(line);
        csvOut.writeColumn(col);
        csvOut.writeColumn(endLine);
        csvOut.writeColumn(endCol);
        csvOut.writeColumn(warning);
        csvOut.writeNewLine();
      }
    }

    csvOut.flush();
    csvOut.close();
  }
  catch ( graph::GraphException e )
  {
    cerr << e.getMessage() << endl;
  }
}

*/

int main(int argc, char *argv[]) {

	MAIN_BEGIN

	MainInit(argc, argv, "-");

	checkInputFiles();
	checkSeparator();
	checkMemStatFile();

	if (inputFiles.size() != 1) {
		clError();
	}

	writeMemStat("Memory usage at the beginnig: ");

	WriteMsg::write(WriteMsg::mlDebug, "Loading input lim\n");

	// lim asg
	RefDistributorStrTable strTable;
	Factory factory(strTable, "", limLangOther); // the language and the root name will be loaded

	lim::asg::OverrideRelations overrides(factory);

	std::list<HeaderData*> header;
	PropertyData prop;
	header.push_back(&prop);
	header.push_back(&overrides);
	factory.load(inputFiles.begin()->c_str(), header);


	factory.initializeFilter();
	loadFilter(factory, inputFiles.begin()->c_str());

	WriteMsg::write(WriteMsg::mlDebug, "Converting lim to graph\n");

	//lim to graph conversion
	graph::Graph inGraph;
	columbus::lim2graph::convertBaseGraph(factory, inGraph, true, true, true, true);

	inGraph.saveXML("saveXML.xml");

	unsigned long memoryUsage = 0;

	// inGraph.saveXML( "saveXMLMetrics.xml" );

	WriteMsg::write(WriteMsg::mlDebug, "Finding antipatterns\n");

	string apfile;
	if (common::pathIsAbsolute(rulfile.c_str())) {
		apfile = rulfile;
	} else {
		apfile = getExecutableProgramDir() + (rulfile != UNDEFINED ? rulfile : DEF_INI_FILE);
	}	
	
	RulHandler aprl(apfile, "Default", "eng");

	AlgorithmPreorder ap;
	ap.setVisitSpecialNodes(true, true);
	ap.setCrossEdgeToTraversal(lim::asg::edkScope_HasMember);
	ap.setSafeMode();

	set<string> patterns;

	auto* stinkyVisitor = new LIM2StinkVisitor(factory, inGraph, patterns, aprl);

	ap.run(factory, *stinkyVisitor, factory.getRoot()->getId());


	delete stinkyVisitor;

	WriteStats(memoryUsage);

	MAIN_END

	return 0;
}
