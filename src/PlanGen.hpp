#ifndef H_PlanGen
#define H_PlanGen

#include "Database.hpp"
#include "Table.hpp"
#include "QueryGraph.hpp"
#include "Exception.hpp"
#include "operator/Operator.hpp"
#include "operator/Tablescan.hpp"
#include "operator/Projection.hpp"
#include "operator/Printer.hpp"
#include "operator/Selection.hpp"
#include "operator/CrossProduct.hpp"
#include "operator/HashJoin.hpp"
#include "Register.hpp"
#include <unordered_map>
#include <map>
#include <vector>
#include <set>
#include <memory>
#include <ostream>
#include <functional>

class PlanGen {
  private:
    QueryGraph& queryGraph;
    Parser::Result& result;
    Database& database;
    std::unordered_map<std::string, Table*> tables;
    std::unordered_map<std::string, std::unique_ptr<Tablescan>> tablescans;
    std::unordered_map<std::string, const Attribute*> attributes;
    std::unordered_map<std::string, const Register*> registers;
    std::vector<const Register*> projection;
    typedef std::vector<std::tuple<std::string, const Register*, const Register*>> SelectionsType;
    SelectionsType selections;
    std::vector<Register> constants;
    typedef std::vector<std::map<std::set<std::string>, Operator*>> DpTableType;
    DpTableType dpTable;
    std::map<std::set<std::string>, float> dpLookup;
    std::map<std::set<std::string>, std::string> dpDebug;

    void loadTables();
    void loadProjections();
    void loadSelections();
    void loadJoinRegisters();
    std::unique_ptr<Operator> addSelections(std::string relation, std::unique_ptr<Operator> op);

    bool canJoin(std::set<std::string> left, std::set<std::string> right, std::vector<QueryGraph::Edge*> edges, float& cost, QueryGraph::Edge** edge);
    float lookupCost(std::set<std::string> relations);
    float getCost(std::set<std::string> left, std::set<std::string> right, QueryGraph::Edge* edge);

  public:
    PlanGen(Database& database, QueryGraph& queryGraph, Parser::Result& result)
      : queryGraph(queryGraph), result(result), database(database) { }
    std::unique_ptr<Operator> generate(std::ostream& explain);

  class GenError : public Exception {
    public:
      GenError(const std::string message) : Exception(message) {
      }
  };
};

#endif
