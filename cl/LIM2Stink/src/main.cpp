#define PROGRAM_NAME "LIM2Stink"
#define EXECUTABLE_NAME PROGRAM_NAME

#include <fstream>
#include <iostream>
#include <lim/inc/lim.h>
#include <ErrorCodes.h>
#include <common/inc/FileSup.h>
#include <ReleaseVersion.h>
#include <common/inc/Stat.h>
#include <common/inc/StringSup.h>
#include <rul/inc/RulHandler.h>
#include <common/inc/WriteMessage.h>
#include <common/inc/Arguments.h>

#include "../inc/LIM2Stink.h"
#include "../inc/visitors/Singleton.h"
#include "../inc/visitors/TemporaryField.h"
#include "../inc/visitors/RefusedBequest.h"

#include <graph/inc/graph.h>
#include <lim2graph/inc/Lim2GraphConverter.h>
#include <graphsupport/inc/CsvExporter.h>
#include <graphsupport/inc/MetricSum.h>
#include <graphsupport/inc/RulBuilder.h>

#include <io/inc/IO.h>

#include "MainCommon.h"

using namespace std;
using namespace common;
using namespace columbus;
using namespace columbus::rul;
using namespace columbus::lim::asg;
using namespace columbus::lim::antipatterns;

// parameters

static string csvFile;
static string graphFile;
static string rulFile = "Stink.rul";
static string rulConfig = "Default";
static string separator = ",";
static bool exportRul = false;
static list<string> inputFiles;


// Callback methods for argument processing

static bool ppCsv( const common::Option *o, char *argv[] ) {
  csvFile = argv[0];
  return true;
}

static bool ppRul( const Option *o, char *argv[] ) {
  rulFile = argv[0];
  return true;
}

static bool ppRulConfig( const Option *o, char *argv[] ) {
  rulConfig = argv[0];
  return true;
}

static bool ppGraph( const Option *o, char *argv[] ) {
  graphFile = argv[0];
  return true;
}

static void ppFile( char *filename ) {
  inputFiles.push_back(filename);
}

static bool ppExportRul( const Option *o, char *argv[] ) {
  exportRul = true;
  return true;
}

static bool ppCsvSeparator( const common::Option *o, char *argv[] ) {
  if ( strcmp( argv[0], "\\t" ) == 0 ) {
    separator = '\t';
  } else if ( argv[0] != 0 ) {
    separator = argv[0][0];
  }
  return true;
}

const Option OPTIONS_OBJ [] = {
  { false, "-graph",        1,  "filename",   0, OT_WC,    ppGraph,      NULL,   "Save binary graph output to the given file"},
  { false, "-csv",          1,  "filename",   0, OT_WC,    ppCsv,        NULL,   "Create csv output file"},
  CL_CSVSEPARATOR
  CL_RUL_AND_RULCONFIG("Stink.rul")
  CL_EXPORTRUL
  COMMON_CL_ARGS
};

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


/*
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

void exportCsv( graph::Graph& graph, const string& filename )
{
  using namespace graph;
  using namespace io;
  if (!filename) {
    return;
  }
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

  if (inputFiles.size() != 1) {
    clError();
  }

  WriteMsg::write(WriteMsg::mlDebug, "Loading input lim\n");

  // Load LIM
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

  // Convert LIM to graph
  graph::Graph limGraph;
  columbus::lim2graph::convertBaseGraph(factory, limGraph, true, true, true, true);

  // Load RUL file
  RulHandler rul( common::indep_fullpath( rulFile ), rulConfig, "eng" );

  AlgorithmPreorder ap;
  ap.setVisitSpecialNodes(true, true);
  ap.setCrossEdgeToTraversal(lim::asg::edkScope_HasMember);
  ap.setSafeMode();
  ap.setFactory(factory);

  std::set<std::shared_ptr<LIM2StinkVisitor>> visitors;

  if (rul.getIsEnabled("Singleton")) {
    visitors.insert(std::make_shared<SingletonVisitor>(factory, limGraph, rul));
    // Add visitors
  }

  if (rul.getIsEnabled("TemporaryField")) {
	  visitors.insert(std::make_shared<TemporaryFieldVisitor>(factory, limGraph, rul));
  }

  if (rul.getIsEnabled("RefusedBequest")) {
	  visitors.insert(std::make_shared<RefusedBequestVisitor>(factory, limGraph, rul, overrides));
  }

  for (auto v : visitors) {
    ap.addVisitor(*v);
  }

  ap.run();

  //exportCsv(limGraph, csvFile);

  if ( ! graphFile.empty() ) {
    cout << "Saving graph to " << graphFile << endl;
    if ( exportRul ) {
      cout << "Exporting rul" << endl;
      graphsupport::buildRulToGraph( limGraph, rul );
    }
    limGraph.saveBinary( graphFile );
  }

  MAIN_END

  return 0;
}
