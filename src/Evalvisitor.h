#ifndef PYTHON_INTERPRETER_EVALVISITOR_H
#define PYTHON_INTERPRETER_EVALVISITOR_H

#include <algorithm>
#include <vector>
#include <map>
#include <cstring>
#include <iostream>
#include <Python3BaseVisitor.h>
#include "Python3Parser.h"
#include "Bigint.h"
using namespace std;
struct arg2default{
    string n;
    antlrcpp::Any d;
};
map<string,antlrcpp::Any> name2value;
map<string,vector<arg2default>> fun2argument;
map<string,Python3Parser::SuiteContext*> fun2ctx;
vector<map<string,antlrcpp::Any>> namestack;
vector<bool> returnj;
bool breakjd = 0,continuejd = 0;
class EvalVisitor: public Python3BaseVisitor {

    antlrcpp::Any visitFile_input(Python3Parser::File_inputContext *ctx) override{
        namestack.push_back(name2value);
        return visitChildren(ctx);
    }

    antlrcpp::Any visitFuncdef(Python3Parser::FuncdefContext *ctx) override{
        vector<arg2default> defargs =  visit(ctx->parameters());
        string funcname = ctx->NAME()->getText();
        Python3Parser::SuiteContext* funcontents = ctx->suite();
        fun2ctx[funcname] = funcontents;
        fun2argument[funcname] = defargs;
        return 0;
    }

    antlrcpp::Any visitParameters(Python3Parser::ParametersContext *ctx) override{
        if (ctx->typedargslist() != nullptr) {
            vector<arg2default> tmparg2default = visit(ctx->typedargslist());
            return tmparg2default;
        }
        else {
            vector<arg2default> emptyarg;
            return emptyarg;
        }
    }

    antlrcpp::Any visitTypedargslist(Python3Parser::TypedargslistContext *ctx) override{
        int defarglistlen = ctx->tfpdef().size();
        int deftestlen = ctx->test().size();
        vector<arg2default> tmparg2default;
        for (int i = 0;i <=  defarglistlen - 1 - deftestlen; ++i){
            tmparg2default.push_back({visit(ctx->tfpdef()[i]).as<string>(), "no_default_value"});
        }
        for (int i =  defarglistlen - deftestlen; i < defarglistlen; ++i){
            tmparg2default.push_back({visit(ctx->tfpdef()[i]).as<string>(),visit(ctx->test()[i - defarglistlen + deftestlen])});
        }
        return tmparg2default;
    }

    antlrcpp::Any visitTfpdef(Python3Parser::TfpdefContext *ctx) override{
        return ctx->NAME()->getText();
    }

    antlrcpp::Any visitStmt(Python3Parser::StmtContext *ctx) override{
        return visitChildren(ctx);
    }

    antlrcpp::Any visitSimple_stmt(Python3Parser::Simple_stmtContext *ctx) override{
        return visitChildren(ctx);
    }

    antlrcpp::Any visitSmall_stmt(Python3Parser::Small_stmtContext *ctx) override{
        return visitChildren(ctx);
    }

    antlrcpp::Any visitExpr_stmt(Python3Parser::Expr_stmtContext *ctx) override{
        if (ctx->testlist().size() == 1){
            return visit(ctx->testlist()[0]->test()[0]);
        }
        if(ctx->augassign() != nullptr) {
            string exprlstr = ctx->testlist()[0]->test()[0]->or_test()->and_test()[0]->not_test()[0]->comparison()->arith_expr()[0]->term()[0]->factor()[0]->atom_expr()->atom()->NAME()->getText();
            string exprstr = visit(ctx->augassign());
            antlrcpp::Any exprl = visit(ctx->testlist()[0]->test()[0]);
            antlrcpp::Any exprr = visit(ctx->testlist()[1]->test()[0]);
            if (exprstr == "+=") {
                if (exprl.is<double>()) {
                    if (exprr.is<double>()) {
                        exprl = exprl.as<double>() + exprr.as<double>();
                    } else if (exprr.is<Bigint>()) {
                        exprl = exprl.as<double>() + double(exprr.as<Bigint>());
                    } else if (exprr.is<bool>()) {
                        exprl = exprl.as<double>() + exprr.as<bool>();
                    }
                } else if (exprr.is<double>()) {
                    if (exprl.is<Bigint>()) {
                        exprl = exprr.as<double>() + double(exprl.as<Bigint>());
                    } else if (exprl.is<bool>()) {
                        exprl = exprr.as<double>() + exprl.as<bool>();
                    }
                } else if (exprl.is<string>()) {
                    exprl = exprl.as<string>() + exprr.as<string>();
                } else if (exprl.is<Bigint>()) {
                    if (exprr.is<Bigint>()) {
                        Bigint test = exprr.as<Bigint>() + exprl.as<Bigint>();
                        exprl = exprr.as<Bigint>() + exprl.as<Bigint>();
                    } else if (exprr.is<bool>()) {
                        exprl = exprl.as<Bigint>() + Bigint(exprr.as<bool>());
                    }
                } else if (exprr.is<Bigint>()) {
                    exprl = exprr.as<Bigint>() + Bigint(exprl.as<bool>());
                } else if (exprl.is<bool>()) {
                    exprl = Bigint(exprr.as<bool>() + exprl.as<bool>());
                }
            } else if (exprstr == "-=") {
                if (exprl.is<double>()) {
                    if (exprr.is<double>()) {
                        exprl = exprl.as<double>() - exprr.as<double>();
                    } else if (exprr.is<Bigint>()) {
                        exprl = exprl.as<double>() - double(exprr.as<Bigint>());
                    } else if (exprr.is<bool>()) {
                        exprl = exprl.as<double>() - double(exprr.as<bool>());
                    }
                } else if (exprr.is<double>()) {
                    if (exprl.is<Bigint>()) {
                        exprl = double(exprl.as<Bigint>()) - exprr.as<double>();
                    } else if (exprl.is<bool>()) {
                        exprl = double(exprl.as<bool>()) - exprr.as<double>();
                    }
                } else if (exprl.is<Bigint>()) {
                    if (exprr.is<Bigint>()) {
                        exprl = exprl.as<Bigint>() - exprr.as<Bigint>();
                    } else if (exprr.is<bool>()) {
                        exprl = exprl.as<Bigint>() - Bigint(exprr.as<bool>());
                    }
                } else if (exprr.is<Bigint>()) {
                    exprl = Bigint(exprl.as<bool>()) - exprr.as<Bigint>();
                } else if (exprl.is<bool>()) {
                    exprl = Bigint(exprl.as<bool>() - exprr.as<bool>());
                }

            } else if (exprstr == "*=") {
                if (exprl.is<double>()){
                    if (exprr.is<double>()){
                        exprl = exprl.as<double>() * exprr.as<double>();
                    }
                    else if (exprr.is<Bigint>()){
                        exprl = exprl.as<double>() * double(exprr.as<Bigint>());
                    }
                    else if (exprr.is<bool>()){
                        exprl = exprl.as<double>() * exprr.as<bool>();
                    }
                }
                else if (exprr.is<double>()){
                    if (exprl.is<Bigint>()){
                        exprl = exprr.as<double>() * double(exprl.as<Bigint>());
                    }
                    else if (exprl.is<bool>()){
                        exprl = exprr.as<double>() * exprl.as<bool>();
                    }
                }
                else if (exprl.is<string>()){
                    string tmpstr = exprl.as<string>();
                    string ansstr = "";
                    if (exprr.is<Bigint>()){
                        for (double i = 0; i < double(exprr.as<Bigint>()); ++i)
                            ansstr = ansstr + tmpstr;
                    }
                    else if (exprr.is<bool>()){
                        for (int i = 0; i < exprr.as<bool>(); ++i)
                            ansstr = ansstr + tmpstr;
                    }
                    exprl = ansstr;
                }
                else if (exprr.is<string>()){
                    string tmpstr = exprr.as<string>();
                    string ansstr;
                    if (exprl.is<Bigint>()) {
                        for (double i = 0; i < double(exprl.as<Bigint>()); ++i)
                            ansstr = ansstr + tmpstr;
                    }
                    else if (exprl.is<bool>()) {
                        for (int i = 0; i < exprl.as<bool>(); ++i)
                            ansstr = ansstr + tmpstr;
                    }
                    exprl = ansstr;
                }
                else if (exprl.is<Bigint>()){
                    if (exprr.is<Bigint>()){
                        exprl = exprr.as<Bigint>() * exprl.as<Bigint>();
                    }
                    else if (exprr.is<bool>()){
                        exprl = exprl.as<Bigint>() * Bigint(exprr.as<bool>());
                    }
                }
                else if (exprr.is<Bigint>()){
                    exprl = exprr.as<Bigint>() * Bigint(exprl.as<bool>());
                }
                else if (exprl.is<bool>()){
                    exprl = exprr.as<bool>() * exprl.as<bool>();
                }
            } else if (exprstr == "/=") {
                if (exprl.is<double>()){
                    if (exprr.is<double>()){
                        exprl = exprl.as<double>() / exprr.as<double>();
                    }
                    else if (exprr.is<Bigint>()){
                        exprl = exprl.as<double>() / double(exprr.as<Bigint>());
                    }
                    else if (exprr.is<bool>()){
                        exprl = exprl.as<double>();
                    }
                }
                else if (exprr.is<double>()){
                    if (exprl.is<Bigint>()){
                        exprl = double(exprl.as<Bigint>()) / exprr.as<double>();
                    }
                    else if (exprl.is<bool>()){
                        exprl = double(exprl.as<bool>()) / exprr.as<double>();
                    }
                }
                else if (exprl.is<Bigint>()){
                    if (exprr.is<Bigint>()){
                        exprl = double(exprl.as<Bigint>()) / double(exprr.as<Bigint>());
                    }
                    else if (exprr.is<bool>()){
                        exprl = exprl.as<Bigint>();
                    }
                }
                else if (exprr.is<Bigint>()){
                    exprl = double(exprl.as<bool>()) / double(exprr.as<Bigint>());
                }
                else if (exprl.is<bool>()){
                    exprl = exprl.as<bool>();
                }
            } else if (exprstr == "//=") {
                if (exprl.is<double>()){
                    if (exprr.is<double>()){
                        exprl = exprl.as<double>() / exprr.as<double>();
                        if (exprl.as<double>() != int(exprl.as<double>())){
                            if (exprl.as<double>() < 0){
                                exprl = double(int(exprl.as<double>()) - 1);
                            } else {
                                exprl = double(int(exprl.as<double>()));
                            }
                        }
                    }
                    else if (exprr.is<Bigint>()){
                        exprl = exprl.as<double>() / double(exprr.as<Bigint>());
                        if (exprl.as<double>() != int(exprl.as<double>())){
                            if (exprl.as<double>() < 0){
                                exprl = double(int(exprl.as<double>()) - 1);
                            } else {
                                exprl = double(int(exprl.as<double>()));
                            }
                        }
                    }
                    else if (exprr.is<bool>()){
                        exprl = exprl.as<double>();
                        if (exprl.as<double>() != int(exprl.as<double>())){
                            if (exprl.as<double>() < 0){
                                exprl = double(int(exprl.as<double>()) - 1);
                            } else {
                                exprl = double(int(exprl.as<double>()));
                            }
                        }
                    }
                }
                else if (exprr.is<double>()){
                    if (exprl.is<Bigint>()){
                        exprl = double(exprl.as<Bigint>()) / exprr.as<double>();
                        if (exprl.as<double>() != int(exprl.as<double>())){
                            if (exprl.as<double>() < 0){
                                exprl = double(int(exprl.as<double>()) - 1);
                            } else {
                                exprl = double(int(exprl.as<double>()));
                            }
                        }
                    }
                    else if (exprl.is<bool>()){
                        exprl = double(exprl.as<bool>()) / exprr.as<double>();
                        if (exprl.as<double>() != int(exprl.as<double>())){
                            if (exprl.as<double>() < 0){
                                exprl = double(int(exprl.as<double>()) - 1);
                            } else {
                                exprl = double(int(exprl.as<double>()));
                            }
                        }
                    }
                }
                else if (exprl.is<Bigint>()){
                    if (exprr.is<Bigint>()){
                        exprl = exprl.as<Bigint>() / exprr.as<Bigint>();
                    }
                    else if (exprr.is<bool>()){
                        exprl = exprl.as<Bigint>();
                    }
                }
                else if (exprr.is<Bigint>()){
                    exprl = Bigint(exprl.as<bool>()) / exprr.as<Bigint>();
                }
                else if (exprl.is<bool>()){
                    exprl = exprl.as<bool>();
                }
            } else if (exprstr == "%=") {
                if (exprl.is<double>()){
                    if (exprr.is<double>()){
                        double qiuyud = exprl.as<double>();
                        exprl = exprl.as<double>() / exprr.as<double>();
                        if (exprl.as<double>() != int(exprl.as<double>())){
                            if (exprl.as<double>() < 0){
                                exprl = double(int(exprl.as<double>()) - 1);
                            } else {
                                exprl = double(int(exprl.as<double>()));
                            }
                        }
                        exprl = qiuyud - exprl.as<double>();
                    }
                    else if (exprr.is<Bigint>()){
                        double qiuyud = exprl.as<double>();
                        exprl = exprl.as<double>() / double(exprr.as<Bigint>());
                        if (exprl.as<double>() != int(exprl.as<double>())){
                            if (exprl.as<double>() < 0){
                                exprl = double(int(exprl.as<double>()) - 1);
                            } else {
                                exprl = double(int(exprl.as<double>()));
                            }
                        }
                        exprl = qiuyud - exprl.as<double>();
                    }
                    else if (exprr.is<bool>()){
                        double qiuyud = exprl.as<double>();
                        exprl = exprl.as<double>();
                        if (exprl.as<double>() != int(exprl.as<double>())){
                            if (exprl.as<double>() < 0){
                                exprl = double(int(exprl.as<double>()) - 1);
                            } else {
                                exprl  = double(int(exprl.as<double>()));
                            }
                        }
                        exprl = qiuyud - exprl.as<double>();
                    }
                }
                else if (exprr.is<double>()){
                    if (exprl.is<Bigint>()){
                        double qiuyud = double(exprl.as<Bigint>());
                        exprl = double(exprl.as<Bigint>()) / exprr.as<double>();
                        if (exprl.as<double>() != int(exprl.as<double>())){
                            if (exprl.as<double>() < 0){
                                exprl = double(int(exprl.as<double>()) - 1);
                            } else {
                                exprl = double(int(exprl.as<double>()));
                            }
                        }
                        exprl = qiuyud - exprl.as<double>();
                    }
                    else if (exprl.is<bool>()){
                        double qiuyud = double(exprl.as<bool>());
                        exprl = double(exprl.as<bool>()) / exprr.as<double>();
                        if (exprl.as<double>() != int(exprl.as<double>())){
                            if (exprl.as<double>() < 0){
                                exprl = double(int(exprl.as<double>()) - 1);
                            } else {
                                exprl = double(int(exprl.as<double>()));
                            }
                        }
                        exprl = qiuyud - exprl.as<double>();
                    }
                }
                else if (exprl.is<Bigint>()){
                    if (exprr.is<Bigint>()){
                        exprl = exprl.as<Bigint>() % exprr.as<Bigint>();
                    }
                    else if (exprr.is<bool>()){
                        exprl = Bigint(0);
                    }
                }
                else if (exprr.is<Bigint>()){
                    exprl = Bigint(exprl.as<bool>()) % exprr.as<Bigint>();
                }
                else if (exprl.is<bool>()){
                    exprl = Bigint(0);
                }
            }
            namestack[namestack.size()-1][exprlstr] = exprl;
            return visitChildren(ctx);
        } else {
            int exprlength = ctx->testlist().size();
            int exprtestlen = ctx->testlist()[0]->test().size();
            if (exprtestlen != ctx->testlist()[1]->test().size()){
                vector<antlrcpp::Any> exprfunans = visit(ctx->testlist()[1]->test()[0]);
                for (int i = 0; i < exprtestlen; ++i){
                    string expralll = ctx->testlist()[0]->test()[i]->or_test()->and_test()[0]->not_test()[0]->comparison()->arith_expr()[0]->term()[0]->factor()[0]->atom_expr()->atom()->NAME()->getText();
                    namestack[namestack.size()-1][expralll] = exprfunans[i];
                }
                return 0;
            }
            for (int i = 0; i < exprtestlen; ++i){
                antlrcpp::Any expronlyr = visit(ctx->testlist()[exprlength - 1]->test()[i]);
                for (int j = 0; j < exprlength - 1; ++j) {
                    string expralll = ctx->testlist()[j]->test()[i]->or_test()->and_test()[0]->not_test()[0]->comparison()->arith_expr()[0]->term()[0]->factor()[0]->atom_expr()->atom()->NAME()->getText();
                    namestack[namestack.size()-1][expralll] = expronlyr;
                    //if (mapName[expralll].is<double>())
                      //  cout << "valueinthemap:" << mapName[expralll].as<double>() << endl;
                }
            }
            return visitChildren(ctx);
        }
    }

    antlrcpp::Any visitAugassign(Python3Parser::AugassignContext *ctx) override{
        string augassignstr = ctx->getText();
        return augassignstr;
    }

    antlrcpp::Any visitFlow_stmt(Python3Parser::Flow_stmtContext *ctx) override{
        return visitChildren(ctx);
    }

    antlrcpp::Any visitBreak_stmt(Python3Parser::Break_stmtContext *ctx) override{
        breakjd = 1;
        return 0;
    }

    antlrcpp::Any visitContinue_stmt(Python3Parser::Continue_stmtContext *ctx) override{
        continuejd = 1;
        return 0;
    }

    antlrcpp::Any visitReturn_stmt(Python3Parser::Return_stmtContext *ctx) override{
        if (ctx->testlist() != nullptr){
            antlrcpp::Any returnfunvalue =  visit(ctx->testlist()->test()[0]);
            if (ctx->testlist()->test().size() == 1) {
                return returnfunvalue;
            }
            return visit(ctx->testlist());
        }
        else return 0;
    }

    antlrcpp::Any visitCompound_stmt(Python3Parser::Compound_stmtContext *ctx) override{
        return visitChildren(ctx);
    }

    antlrcpp::Any visitIf_stmt(Python3Parser::If_stmtContext *ctx) override{
        int iflen = ctx->test().size();
        for (int i = 0; i < iflen; ++i) {
            antlrcpp::Any iftest = visit(ctx->test(i));
            if (!iftest.is<Nonetype>() && !(iftest.is<double>() && iftest.as<double>() == 0) &&
                !(iftest.is<Bigint>() && iftest.as<Bigint>() == Bigint(0)) &&
                !(iftest.is<bool>() && iftest.as<bool>() == 0) &&
                !(iftest.is<string>() && iftest.as<string>() == "")) {
                return visit(ctx->suite()[i]);
            }
        }
        if (ctx->ELSE() != nullptr) {
            return visit(ctx->suite()[iflen]);
        }
        return 0;
    }

    antlrcpp::Any visitWhile_stmt(Python3Parser::While_stmtContext *ctx) override{
        while(!breakjd) {
            continuejd = 0;
            antlrcpp::Any whiletest = visit(ctx->test());
            if (!whiletest.is<Nonetype>() && !(whiletest.is<double>() && whiletest.as<double>() == 0) &&
                !(whiletest.is<Bigint>() && whiletest.as<Bigint>() == Bigint(0)) &&
                !(whiletest.is<bool>() && whiletest.as<bool>() == 0) &&
                !(whiletest.is<string>() && whiletest.as<string>() == "")) {
                antlrcpp::Any tmpwhile = visit(ctx->suite());
                if (returnj.size() > 0 && returnj[returnj.size() - 1] == 1){
                    return  tmpwhile;
                }
            } else {
                breakjd = 0;
                return 0;
            }
        }
        breakjd = 0;
        return 0;
    }

    antlrcpp::Any visitSuite(Python3Parser::SuiteContext *ctx) override{
        if (ctx->simple_stmt() != nullptr) {
            if (ctx->simple_stmt()->small_stmt()->flow_stmt()!= nullptr){
                if (ctx->simple_stmt()->small_stmt()->flow_stmt()->return_stmt()!= nullptr){
                    if (returnj.size()>0)returnj[returnj.size() - 1] = 1;
                    return visit(ctx->simple_stmt()->small_stmt()->flow_stmt()->return_stmt());
                }
            }
            visit(ctx->simple_stmt());
            if (returnj.size()>0)returnj[returnj.size() - 1] = 0;
            return 0;
        } else {
            for (int i = 0; i < ctx->stmt().size(); ++i){
                if (ctx->stmt()[i]->simple_stmt() != nullptr){
                    if (ctx->stmt()[i]->simple_stmt()->small_stmt()->flow_stmt()!= nullptr){
                        if (ctx->stmt()[i]->simple_stmt()->small_stmt()->flow_stmt()->return_stmt()!= nullptr){
                            if (returnj.size()>0)returnj[returnj.size() - 1] = 1;
                            return visit(ctx->stmt()[i]->simple_stmt()->small_stmt()->flow_stmt()->return_stmt());
                        }
                    }
                    //cout << "will visit simplestmt" << i << "in" << ctx->stmt().size() << endl;
                    visit(ctx->stmt()[i]->simple_stmt());
                    if (returnj.size()>0) returnj[returnj.size() - 1] = 0;
                } else {
                    antlrcpp::Any tmpcompound = visit(ctx->stmt()[i]->compound_stmt());
                    if (returnj.size()>0 && returnj[returnj.size() - 1]) return tmpcompound;
                }
                if (continuejd == 1 || breakjd == 1){
                    continuejd = 0;
                    return 0;
                }
            }
            return 0;
        }
    }

    antlrcpp::Any visitTest(Python3Parser::TestContext *ctx) override{
        return visitChildren(ctx);
    }

    antlrcpp::Any visitOr_test(Python3Parser::Or_testContext *ctx) override{
        if (ctx->and_test().size() == 1){
            return visit(ctx->and_test(0));
        }
        bool ortest = 1;
        for (int i = 0; i < ctx->and_test().size(); ++i){
            if (visit(ctx->and_test(i))) return ortest;
        }
        ortest = 0;
        return ortest;
    }

    antlrcpp::Any visitAnd_test(Python3Parser::And_testContext *ctx) override{
        if (ctx->not_test().size() == 1){
            return visit(ctx->not_test(0));
        }
        bool andtest = 0;
        for (int i = 0 ; i < ctx->not_test().size(); ++i){
            if (!visit(ctx->not_test(i))) return andtest;
        }
        andtest = 1;
        return andtest;
    }

    antlrcpp::Any visitNot_test(Python3Parser::Not_testContext *ctx) override{
        if (ctx->comparison() != nullptr){
            return visit(ctx->comparison());
        } else {
            antlrcpp::Any comp2not = visit(ctx->not_test());
            if (comp2not.is<bool>()){
                return !comp2not.as<bool>();
            }
            else if (comp2not.is<Bigint>()) {
                if (Bjudge(comp2not.as<Bigint>(), Bigint(0)) && Bjudge(comp2not.as<Bigint>(), Bigint(0))) {
                    return true;
                } else {
                    return false;
                }
            }
            else if (comp2not.is<double>()){
                if (comp2not.as<double>() == 0){
                    return true;
                } else {
                    return false;
                }
            }
            else if (comp2not.is<string>()){
                if (comp2not.as<string>() == ""){
                    return true;
                } else {
                    return false;
                }
            }
        }
    }

    antlrcpp::Any visitComparison(Python3Parser::ComparisonContext *ctx) override{
        bool comp = 0;
        if (ctx->comp_op().size() == 0){
            return visit(ctx->arith_expr(0));
        }
        vector<antlrcpp::Any> compvector;
        for (int i = 0; i <= ctx->comp_op().size(); ++i) {
            compvector.push_back(visit(ctx->arith_expr(i)));
        }
        for (int i = 0; i < ctx->comp_op().size(); ++i) {
            string compstr = visit(ctx->comp_op(i));
            antlrcpp::Any retl = compvector[i],retr = compvector[i + 1];
            if (compstr == ">"){
                if (retl.is<string>()) {
                    const char* retcharl = retl.as<string>().c_str();
                    const char* retcharr = retr.as<string>().c_str();
                    if (strcmp(retcharl,retcharr) <= 0){
                        return comp;
                    }
                }
                else if (retl.is<double>()) {
                    if (retr.is<double>() && retl.as<double>() <= retr.as<double>()){
                        return comp;
                    }
                    else if (retr.is<Bigint>() && retl.as<double>() <= double(retr.as<Bigint>())){
                        return comp;
                    }
                    else if (retr.is<bool>() && retl.as<double>() <= retr.as<bool>()){
                        return comp;
                    }
                }
                else if (retl.is<Bigint>()) {
                    if (retr.is<Bigint>()&&Bjudge(retr.as<Bigint>(),retl.as<Bigint>())){
                        return comp;
                    }
                    else if (retr.is<double>()&&Bjudge(Bigint(retr.as<double>()),retl.as<Bigint>())){
                        return comp;
                    }
                    else if (retr.is<bool>()&&Bjudge(Bigint(retr.as<bool>()),retl.as<Bigint>())){
                        return comp;
                    }
                }
                else if (retl.is<bool>()) {
                    if (retr.is<bool>()&&retl.as<bool>() <= retr.as<bool>()){
                        return comp;
                    }
                    else if (retr.is<Bigint>()&&Bjudge(retr.as<Bigint>(),Bigint(retl.as<bool>()))){
                        return comp;
                    }
                    else if (retr.is<double>()&&retr.as<double>()>=retl.as<bool>()){
                        return comp;
                    }
                }
            }
            else if (compstr == "<") {
                if (retl.is<string>()) {
                    const char* retcharl = retl.as<string>().c_str();
                    const char* retcharr = retr.as<string>().c_str();
                    if (strcmp(retcharl,retcharr) >= 0){
                        return comp;
                    }
                }
                else if (retl.is<double>()) {
                    if (retr.is<double>() && retl.as<double>() >= retr.as<double>()){
                        return comp;
                    }
                    else if (retr.is<Bigint>() && retl.as<double>() >= double(retr.as<Bigint>())){
                        return comp;
                    }
                    else if (retr.is<bool>() && retl.as<double>() >= retr.as<bool>()){
                        return comp;
                    }
                }
                else if (retl.is<Bigint>()) {
                    if (retr.is<Bigint>()&&Bjudge(retl.as<Bigint>(),retr.as<Bigint>())){
                        return comp;
                    }
                    else if (retr.is<double>()&&Bjudge(retl.as<Bigint>(),Bigint(retr.as<double>()))){
                        return comp;
                    }
                    else if (retr.is<bool>()&&Bjudge(retl.as<Bigint>(),Bigint(retr.as<bool>()))){
                        return comp;
                    }
                }
                else if (retl.is<bool>()) {
                    if (retr.is<bool>()&&retl.as<bool>() >= retr.as<bool>()){
                        return comp;
                    }
                    else if (retr.is<Bigint>()&&Bjudge(Bigint(retl.as<bool>()),retr.as<Bigint>())){
                        return comp;
                    }
                    else if (retr.is<double>()&&retl.as<double>()>=retr.as<bool>()){
                        return comp;
                    }
                }
            }
            else if (compstr == "==") {
                if (retl.is<string>()) {
                    if (!retr.is<string>()) return comp;
                    const char* retcharl = retl.as<string>().c_str();
                    const char* retcharr = retr.as<string>().c_str();
                    if (strcmp(retcharl,retcharr) != 0){
                        return comp;
                    }
                }
                else if (retl.is<double>()) {
                    if (retr.is<double>() && retl.as<double>() != retr.as<double>()){
                        return comp;
                    }
                    else if (retr.is<Bigint>() && retl.as<double>() != double(retr.as<Bigint>())){
                        return comp;
                    }
                    else if (retr.is<bool>() && retl.as<double>() != retr.as<bool>()){
                        return comp;
                    }
                }
                else if (retl.is<Bigint>()) {
                    if (retr.is<Bigint>()&&!(retr.as<Bigint>()==retl.as<Bigint>())){
                        return comp;
                    }
                    else if (retr.is<double>()&&!(Bigint(retr.as<double>())==retl.as<Bigint>())){
                        return comp;
                    }
                    else if (retr.is<bool>()&&!(Bigint(retr.as<bool>())==retl.as<Bigint>())){
                        return comp;
                    }
                }
                else if (retl.is<bool>()) {
                    if (retr.is<bool>()&&retl.as<bool>() != retr.as<bool>()){
                        return comp;
                    }
                    else if (retr.is<Bigint>()&&!(retr.as<Bigint>()==Bigint(retl.as<bool>()))){
                        return comp;
                    }
                    else if (retr.is<double>()&&retr.as<double>()!=retl.as<bool>()){
                        return comp;
                    }
                }
                else if (retl.is<Nonetype>() && !retr.is<Nonetype>() || retr.is<Nonetype>() && !retl.is<Nonetype>() ) {
                        return comp;
                }
                if (!retl.is<string>() && retr.is<string>()) return comp;
            }
            else if (compstr == ">=") {
                if (retl.is<string>()) {
                    const char* retcharl = retl.as<string>().c_str();
                    const char* retcharr = retr.as<string>().c_str();
                    if (strcmp(retcharl,retcharr) < 0){
                        return comp;
                    }
                }
                else if (retl.is<double>()) {
                    if (retr.is<double>() && retl.as<double>() < retr.as<double>()){
                        return comp;
                    }
                    else if (retr.is<Bigint>() && retl.as<double>() < double(retr.as<Bigint>())){
                        return comp;
                    }
                    else if (retr.is<bool>() && retl.as<double>() < retr.as<bool>()){
                        return comp;
                    }
                }
                else if (retl.is<Bigint>()) {
                    if (retr.is<Bigint>()&&!Bjudge(retl.as<Bigint>(),retr.as<Bigint>())){
                        return comp;
                    }
                    else if (retr.is<double>()&&!Bjudge(retl.as<Bigint>(),Bigint(retr.as<double>()))){
                        return comp;
                    }
                    else if (retr.is<bool>()&&!Bjudge(retl.as<Bigint>(),Bigint(retr.as<bool>()))){
                        return comp;
                    }
                }
                else if (retl.is<bool>()) {
                    if (retr.is<bool>()&&retl.as<bool>() < retr.as<bool>()){
                        return comp;
                    }
                    else if (retr.is<Bigint>()&&!Bjudge(Bigint(retl.as<bool>()),retr.as<Bigint>())){
                        return comp;
                    }
                    else if (retr.is<double>()&&retr.as<double>()>retl.as<bool>()){
                        return comp;
                    }
                }
            }
            else if (compstr == "<=") {
                if (retl.is<string>()) {
                    const char* retcharl = retl.as<string>().c_str();
                    const char* retcharr = retr.as<string>().c_str();
                    if (strcmp(retcharl,retcharr) > 0){
                        return comp;
                    }
                }
                else if (retl.is<double>()) {
                    if (retr.is<double>() && retl.as<double>() > retr.as<double>()){
                        return comp;
                    }
                    else if (retr.is<Bigint>() && retl.as<double>() > double(retr.as<Bigint>())){
                        return comp;
                    }
                    else if (retr.is<bool>() && retl.as<double>() > retr.as<bool>()){
                        return comp;
                    }
                }
                else if (retl.is<Bigint>()) {
                    if (retr.is<Bigint>()&&!Bjudge(retr.as<Bigint>(),retl.as<Bigint>())){
                        return comp;
                    }
                    else if (retr.is<double>()&&!Bjudge(Bigint(retr.as<double>()),retl.as<Bigint>())){
                        return comp;
                    }
                    else if (retr.is<bool>()&&!Bjudge(Bigint(retr.as<bool>()),retl.as<Bigint>())){
                        return comp;
                    }
                }
                else if (retl.is<bool>()) {
                    if (retr.is<bool>()&&retl.as<bool>() > retr.as<bool>()){
                        return comp;
                    }
                    else if (retr.is<Bigint>()&&!Bjudge(retr.as<Bigint>(),Bigint(retl.as<bool>()))){
                        return comp;
                    }
                    else if (retr.is<double>()&&retr.as<double>() < retl.as<bool>()){
                        return comp;
                    }
                }
            }
            else if (compstr == "!=") {
                if (retl.is<string>()) {
                    const char* retcharl = retl.as<string>().c_str();
                    const char* retcharr = retr.as<string>().c_str();
                    if (strcmp(retcharl,retcharr) == 0){
                        return comp;
                    }
                }
                else if (retl.is<double>()) {
                    if (retr.is<double>() && retl.as<double>() == retr.as<double>()){
                        return comp;
                    }
                    else if (retr.is<Bigint>() && retl.as<double>() == double(retr.as<Bigint>())){
                        return comp;
                    }
                    else if (retr.is<bool>() && retl.as<double>() == retr.as<bool>()){
                        return comp;
                    }
                }
                else if (retl.is<Bigint>()) {
                    if (retr.is<Bigint>()&&retr.as<Bigint>()==retl.as<Bigint>()){
                        return comp;
                    }
                    else if (retr.is<double>()&&Bigint(retr.as<double>())==retl.as<Bigint>()){
                        return comp;
                    }
                    else if (retr.is<bool>()&&Bigint(retr.as<bool>())==retl.as<Bigint>()){
                        return comp;
                    }
                }
                else if (retl.is<bool>()) {
                    if (retr.is<bool>()&&retl.as<bool>() == retr.as<bool>()){
                        return comp;
                    }
                    else if (retr.is<Bigint>()&&retr.as<Bigint>()==Bigint(retl.as<bool>())){
                        return comp;
                    }
                    else if (retr.is<double>()&&retr.as<double>()==retl.as<bool>()){
                        return comp;
                    }
                }
                else if (retl.is<Nonetype>() && retr.is<Nonetype>()) {
                    return comp;
                }
            }
        }
        comp = 1;
        return comp;
    }

    antlrcpp::Any visitComp_op(Python3Parser::Comp_opContext *ctx) override{
        string compopstr = ctx->getText();
        return compopstr;
    }

    antlrcpp::Any visitArith_expr(Python3Parser::Arith_exprContext *ctx) override{
        int arith_exprlength = ctx->term().size(),addlength = ctx->ADD().size(),minuslength = ctx->MINUS().size();
        if (arith_exprlength == 1) return visit(ctx->term()[0]);
        map<size_t ,string> maparith_expr;
        size_t arith_exprarray[10000];
        for (int i = 0; i < addlength; ++i){
            maparith_expr[ctx->ADD()[i]->getSymbol()->getTokenIndex()] = "+";
            arith_exprarray[i] = ctx->ADD()[i]->getSymbol()->getTokenIndex();
        }
        for (int i = 0; i < minuslength; ++i){
            maparith_expr[ctx->MINUS()[i]->getSymbol()->getTokenIndex()] = "-";
            arith_exprarray[i + addlength] = ctx->MINUS()[i]->getSymbol()->getTokenIndex();
        }
        sort(arith_exprarray,arith_exprarray + arith_exprlength - 1);

        antlrcpp::Any ansarith_expr = visit(ctx->term()[0]);
        for (int i = 1 ; i < arith_exprlength; ++i) {
            antlrcpp::Any ret = visit(ctx->term()[i]);
            if (maparith_expr[arith_exprarray[i - 1]] == "+") {
                if (ansarith_expr.is<double>()) {
                    if (ret.is<double>()) {
                        ansarith_expr = ansarith_expr.as<double>() + ret.as<double>();
                    } else if (ret.is<Bigint>()) {
                        ansarith_expr = ansarith_expr.as<double>() + double(ret.as<Bigint>());
                    } else if (ret.is<bool>()) {
                        ansarith_expr = ansarith_expr.as<double>() + ret.as<bool>();
                    }
                } else if (ret.is<double>()) {
                    if (ansarith_expr.is<Bigint>()) {
                        ansarith_expr = ret.as<double>() + double(ansarith_expr.as<Bigint>());
                    } else if (ansarith_expr.is<bool>()) {
                        ansarith_expr = ret.as<double>() + ansarith_expr.as<bool>();
                    }
                } else if (ansarith_expr.is<string>()) {
                    ansarith_expr = ansarith_expr.as<string>() + ret.as<string>();
                } else if (ansarith_expr.is<Bigint>()) {
                    if (ret.is<Bigint>()) {
                        ansarith_expr = ret.as<Bigint>() + ansarith_expr.as<Bigint>();
                    } else if (ret.is<bool>()) {
                        ansarith_expr = ansarith_expr.as<Bigint>() + Bigint(ret.as<bool>());
                    }
                } else if (ret.is<Bigint>()) {
                    ansarith_expr = ret.as<Bigint>() + Bigint(ansarith_expr.as<bool>());
                } else if (ansarith_expr.is<bool>()) {
                    ansarith_expr = Bigint(ret.as<bool>() + ansarith_expr.as<bool>());
                }
            } else if (maparith_expr[arith_exprarray[i - 1]] == "-") {
                if (ansarith_expr.is<double>()) {
                    if (ret.is<double>()) {
                        //cout << "double-double"<<endl;
                        ansarith_expr = ansarith_expr.as<double>() - ret.as<double>();
                    } else if (ret.is<Bigint>()) {
                        ansarith_expr = ansarith_expr.as<double>() - double(ret.as<Bigint>());
                    } else if (ret.is<bool>()) {
                        ansarith_expr = ansarith_expr.as<double>() - double(ret.as<bool>());
                    }
                } else if (ret.is<double>()) {
                    if (ansarith_expr.is<Bigint>()) {
                        ansarith_expr = double(ansarith_expr.as<Bigint>()) - ret.as<double>();
                    } else if (ansarith_expr.is<bool>()) {
                        ansarith_expr = double(ansarith_expr.as<bool>()) - ret.as<double>();
                    }
                } else if (ansarith_expr.is<Bigint>()) {
                    if (ret.is<Bigint>()) {
                        ansarith_expr = ansarith_expr.as<Bigint>() - ret.as<Bigint>();
                    } else if (ret.is<bool>()) {
                        ansarith_expr = ansarith_expr.as<Bigint>() - Bigint(ret.as<bool>());
                    }
                } else if (ret.is<Bigint>()) {
                    ansarith_expr = Bigint(ansarith_expr.as<bool>()) - ret.as<Bigint>();
                } else if (ansarith_expr.is<bool>()) {
                    ansarith_expr = Bigint(ansarith_expr.as<bool>() - ret.as<bool>());
                }
            }
        }
        return ansarith_expr;
    }

    antlrcpp::Any visitTerm(Python3Parser::TermContext *ctx) override{
        int termlength = ctx->factor().size(),starlength = ctx->STAR().size(),divlength = ctx->DIV().size(),idivlength = ctx->IDIV().size(),modlength = ctx->MOD().size();
        if (termlength == 1) return visit(ctx->factor()[0]);
        map<size_t ,string> mapterm;
        size_t termarray[10000];
        for (int i = 0; i < starlength; ++i){
            mapterm[ctx->STAR()[i]->getSymbol()->getTokenIndex()] = "*";
            termarray[i] = ctx->STAR()[i]->getSymbol()->getTokenIndex();
        }
        for (int i = 0; i < divlength; ++i){
            mapterm[ctx->DIV()[i]->getSymbol()->getTokenIndex()] = "/";
            termarray[i + starlength] = ctx->DIV()[i]->getSymbol()->getTokenIndex();
        }
        for (int i = 0; i < idivlength; ++i){
            mapterm[ctx->IDIV()[i]->getSymbol()->getTokenIndex()] = "//";
            termarray[i + starlength + divlength] = ctx->IDIV()[i]->getSymbol()->getTokenIndex();
        }
        for (int i = 0; i < modlength; ++i){
            mapterm[ctx->MOD()[i]->getSymbol()->getTokenIndex()] = "%";
            termarray[i + starlength + divlength + idivlength] = ctx->MOD()[i]->getSymbol()->getTokenIndex();
        }
        sort(termarray,termarray + termlength - 1);
        vector<antlrcpp::Any> tmptermvec;
        for (int i = 0 ; i < termlength; ++i){
            tmptermvec.push_back(visit(ctx->factor()[i]));
        }
        antlrcpp::Any ansterm = tmptermvec[0];
        for (int i = 1 ; i < termlength; ++i){
            antlrcpp::Any ret = tmptermvec[i];
            if (mapterm[termarray[i - 1]] == "*"){
                if (ansterm.is<double>()){
                    if (ret.is<double>()){
                        ansterm = ansterm.as<double>() * ret.as<double>();
                    }
                    else if (ret.is<Bigint>()){
                        ansterm = ansterm.as<double>() * double(ret.as<Bigint>());
                    }
                    else if (ret.is<bool>()){
                        ansterm = ansterm.as<double>() * ret.as<bool>();
                    }
                }
                else if (ret.is<double>()){
                    if (ansterm.is<Bigint>()){
                        ansterm = ret.as<double>() * double(ansterm.as<Bigint>());
                    }
                    else if (ansterm.is<bool>()){
                        ansterm = ret.as<double>() * ansterm.as<bool>();
                    }
                }
                else if (ansterm.is<string>()){
                    string tmpstr = ansterm.as<string>();
                    string ansstr = "";
                    if (ret.is<Bigint>()){
                        for (double i = 0; i < double(ret.as<Bigint>()); ++i)
                            ansstr = ansstr + tmpstr;
                    }
                    else if (ret.is<bool>()){
                        for (int i = 0; i < ret.as<bool>(); ++i)
                            ansstr = ansstr + tmpstr;
                    }
                    ansterm = ansstr;
                }
                else if (ret.is<string>()){
                    string tmpstr = ret.as<string>();
                    string ansstr;
                    if (ansterm.is<Bigint>()) {
                        for (double i = 0; i < double(ansterm.as<Bigint>()); ++i)
                            ansstr = ansstr + tmpstr;
                    }
                    else if (ansterm.is<bool>()) {
                        for (int i = 0; i < ansterm.as<bool>(); ++i)
                            ansstr = ansstr + tmpstr;
                    }
                    ansterm = ansstr;
                }
                else if (ansterm.is<Bigint>()){
                    if (ret.is<Bigint>()){
                        ansterm = ret.as<Bigint>() * ansterm.as<Bigint>();
                    }
                    else if (ret.is<bool>()){
                        ansterm = ansterm.as<Bigint>() * Bigint(ret.as<bool>());
                    }
                }
                else if (ret.is<Bigint>()){
                    ansterm = ret.as<Bigint>() * Bigint(ansterm.as<bool>());
                }
                else if (ansterm.is<bool>()){
                    ansterm = ret.as<bool>() * ansterm.as<bool>();
                }
            }
            else if (mapterm[termarray[i - 1]] == "/"){
                if (ansterm.is<double>()){
                    if (ret.is<double>()){
                        ansterm = ansterm.as<double>() / ret.as<double>();
                    }
                    else if (ret.is<Bigint>()){
                        ansterm = ansterm.as<double>() / double(ret.as<Bigint>());
                    }
                    else if (ret.is<bool>()){
                        ansterm = ansterm.as<double>();
                    }
                }
                else if (ret.is<double>()){
                    if (ansterm.is<Bigint>()){
                        ansterm = double(ansterm.as<Bigint>()) / ret.as<double>();
                    }
                    else if (ansterm.is<bool>()){
                        ansterm = double(ansterm.as<bool>()) / ret.as<double>();
                    }
                }
                else if (ansterm.is<Bigint>()){
                    if (ret.is<Bigint>()){
                        ansterm = double(ansterm.as<Bigint>()) / double(ret.as<Bigint>());
                    }
                    else if (ret.is<bool>()){
                        ansterm = ansterm.as<Bigint>();
                    }
                }
                else if (ret.is<Bigint>()){
                    ansterm = double(ansterm.as<bool>()) / double(ret.as<Bigint>());
                }
                else if (ansterm.is<bool>()){
                    ansterm = ansterm.as<bool>();
                }
            }
            else if (mapterm[termarray[i - 1]] == "//"){
                if (ansterm.is<double>()){
                    if (ret.is<double>()){
                        ansterm = ansterm.as<double>() / ret.as<double>();
                        if (ansterm.as<double>() != int(ansterm.as<double>())){
                            if (ansterm.as<double>() < 0){
                                ansterm = double(int(ansterm.as<double>()) - 1);
                            } else {
                                ansterm = double(int(ansterm.as<double>()));
                            }
                        }
                    }
                    else if (ret.is<Bigint>()){
                        ansterm = ansterm.as<double>() / double(ret.as<Bigint>());
                        if (ansterm.as<double>() != int(ansterm.as<double>())){
                            if (ansterm.as<double>() < 0){
                                ansterm = double(int(ansterm.as<double>()) - 1);
                            } else {
                                ansterm = double(int(ansterm.as<double>()));
                            }
                        }
                    }
                    else if (ret.is<bool>()){
                        ansterm = ansterm.as<double>();
                        if (ansterm.as<double>() != int(ansterm.as<double>())){
                            if (ansterm.as<double>() < 0){
                                ansterm = double(int(ansterm.as<double>()) - 1);
                            } else {
                                ansterm = double(int(ansterm.as<double>()));
                            }
                        }
                    }
                }
                else if (ret.is<double>()){
                    if (ansterm.is<Bigint>()){
                        ansterm = double(ansterm.as<Bigint>()) / ret.as<double>();
                        if (ansterm.as<double>() != int(ansterm.as<double>())){
                            if (ansterm.as<double>() < 0){
                                ansterm = double(int(ansterm.as<double>()) - 1);
                            } else {
                                ansterm = double(int(ansterm.as<double>()));
                            }
                        }
                    }
                    else if (ansterm.is<bool>()){
                        ansterm = double(ansterm.as<bool>()) / ret.as<double>();
                        if (ansterm.as<double>() != int(ansterm.as<double>())){
                            if (ansterm.as<double>() < 0){
                                ansterm = double(int(ansterm.as<double>()) - 1);
                            } else {
                                ansterm = double(int(ansterm.as<double>()));
                            }
                        }
                    }
                }
                else if (ansterm.is<Bigint>()){
                    if (ret.is<Bigint>()){
                        ansterm = ansterm.as<Bigint>() / ret.as<Bigint>();
                    }
                    else if (ret.is<bool>()){
                        ansterm = ansterm.as<Bigint>();
                    }
                }
                else if (ret.is<Bigint>()){
                    ansterm = Bigint(ansterm.as<bool>()) / ret.as<Bigint>();
                }
                else if (ansterm.is<bool>()){
                    ansterm = ansterm.as<bool>();
                }
            }
            else if (mapterm[termarray[i - 1]] == "%"){
                if (ansterm.is<double>()){
                    if (ret.is<double>()){
                        double qiuyud = ansterm.as<double>();
                        ansterm = ansterm.as<double>() / ret.as<double>();
                        if (ansterm.as<double>() != int(ansterm.as<double>())){
                            if (ansterm.as<double>() < 0){
                                ansterm = double(int(ansterm.as<double>()) - 1);
                            } else {
                                ansterm = double(int(ansterm.as<double>()));
                            }
                        }
                        ansterm = qiuyud - ansterm.as<double>();
                    }
                    else if (ret.is<Bigint>()){
                        double qiuyud = ansterm.as<double>();
                        ansterm = ansterm.as<double>() / double(ret.as<Bigint>());
                        if (ansterm.as<double>() != int(ansterm.as<double>())){
                            if (ansterm.as<double>() < 0){
                                ansterm = double(int(ansterm.as<double>()) - 1);
                            } else {
                                ansterm = double(int(ansterm.as<double>()));
                            }
                        }
                        ansterm = qiuyud - ansterm.as<double>();
                    }
                    else if (ret.is<bool>()){
                        double qiuyud = ansterm.as<double>();
                        ansterm = ansterm.as<double>();
                        if (ansterm.as<double>() != int(ansterm.as<double>())){
                            if (ansterm.as<double>() < 0){
                                ansterm = double(int(ansterm.as<double>()) - 1);
                            } else {
                                ansterm = double(int(ansterm.as<double>()));
                            }
                        }
                        ansterm = qiuyud - ansterm.as<double>();
                    }
                }
                else if (ret.is<double>()){
                    if (ansterm.is<Bigint>()){
                        double qiuyud = double(ansterm.as<Bigint>());
                        ansterm = double(ansterm.as<Bigint>()) / ret.as<double>();
                        if (ansterm.as<double>() != int(ansterm.as<double>())){
                            if (ansterm.as<double>() < 0){
                                ansterm = double(int(ansterm.as<double>()) - 1);
                            } else {
                                ansterm = double(int(ansterm.as<double>()));
                            }
                        }
                        ansterm = qiuyud - ansterm.as<double>();
                    }
                    else if (ansterm.is<bool>()){
                        double qiuyud = double(ansterm.as<bool>());
                        ansterm = double(ansterm.as<bool>()) / ret.as<double>();
                        if (ansterm.as<double>() != int(ansterm.as<double>())){
                            if (ansterm.as<double>() < 0){
                                ansterm = double(int(ansterm.as<double>()) - 1);
                            } else {
                                ansterm = double(int(ansterm.as<double>()));
                            }
                        }
                        ansterm = qiuyud - ansterm.as<double>();
                    }
                }
                else if (ansterm.is<Bigint>()){
                    if (ret.is<Bigint>()){
                        ansterm = ansterm.as<Bigint>() % ret.as<Bigint>();
                    }
                    else if (ret.is<bool>()){
                        ansterm = Bigint(0);
                    }
                }
                else if (ret.is<Bigint>()){
                    ansterm = Bigint(ansterm.as<bool>()) % ret.as<Bigint>();
                }
                else if (ansterm.is<bool>()){
                    ansterm = Bigint(0);
                }
            }
        }
        return ansterm;
    }

    antlrcpp::Any visitFactor(Python3Parser::FactorContext *ctx) override {
        if (ctx->atom_expr() != nullptr) {
            return visit(ctx->atom_expr());
        } else {
            antlrcpp::Any ret = visit(ctx->factor());
            int flag = ctx->ADD() != nullptr ? 1 : -1;
            if (ret.is<bool>()) {
                return Bigint(int(ret.as<bool>())) * Bigint(flag);
            }
            else if (ret.is<double>()) {
                return ret.as<double>() * flag;
            }
            else if (ret.is<Bigint>()) {
                return ret.as<Bigint>() * Bigint(flag);
            }
        }
    }

    antlrcpp::Any visitAtom_expr(Python3Parser::Atom_exprContext *ctx) override{
        if (ctx->trailer() != nullptr) {
            antlrcpp::Any atomexpr = visit(ctx->atom());
            vector<antlrcpp::Any> trailerv = visit(ctx->trailer());
            if (fun2argument.count(atomexpr.as<string>()) != 0){
                map<string,antlrcpp::Any> tmpfunnamemap;
                returnj.push_back(0);
                namestack.push_back(tmpfunnamemap);
                Python3Parser::SuiteContext* finddef = fun2ctx[atomexpr.as<string>()];
                vector<arg2default> getvalue = fun2argument[atomexpr.as<string>()];
                int trailervlen = trailerv.size(),getvaluelen = getvalue.size();
                for (int i = 0; i < trailervlen; ++i){
                    namestack[namestack.size() - 1][getvalue[i].n] = trailerv[i];
                }
                for (int i = trailervlen; i < getvaluelen; ++i){
                    namestack[namestack.size() - 1][getvalue[i].n] = getvalue[i].d;
                }
                antlrcpp::Any tmpreturn = visit(finddef);
                if (returnj.size()>0 && returnj[returnj.size() - 1]){
                    if (namestack.size() != 1) returnj.pop_back();
                    else returnj[0] = 0;
                    if (namestack.size() != 1) namestack.pop_back();
                    /*cout <<"level "<<namestack.size()-1<<" gonna return : ";
                    if (tmpreturn.is<Bigint>())tmpreturn.as<Bigint>().printB();
                    cout << endl;*/
                    return tmpreturn;
                } else {
                    namestack.pop_back();
                    return 0;
                }
            }
            if (atomexpr.is<string>()&& atomexpr.as<string>() == "print") {
                for (int i = 0; i < trailerv.size(); ++i){
                    antlrcpp::Any tmpatomexpr = trailerv[i];
                    if (tmpatomexpr.is<Bigint>()){
                        //cout <<"Wow, it is a Bigint: ";
                        tmpatomexpr.as<Bigint>().printB();
                    }
                    else if (tmpatomexpr.is<double>()){
                        cout << fixed << setprecision(6) << tmpatomexpr.as<double>();
                    }
                    else if (tmpatomexpr.is<string>()){
                            cout << tmpatomexpr.as<string>();
                        }
                    else if (tmpatomexpr.is<Nonetype>()){
                        cout << "None";
                    }
                    else if (tmpatomexpr.is<bool>()){
                        if (tmpatomexpr.as<bool>()) cout << "True";
                        else cout << "False";
                    } else {
                        vector<antlrcpp::Any> printfunans = trailerv[i];
                        for (int j = 0; j < printfunans.size(); ++j){
                            antlrcpp::Any tmpfunexpr = printfunans[j];
                            if (tmpfunexpr.is<Bigint>()){
                                //cout <<"Wow, it is a Bigint: ";
                                tmpfunexpr.as<Bigint>().printB();
                            }
                            else if (tmpfunexpr.is<double>()){
                                cout << fixed << setprecision(6) << tmpfunexpr.as<double>();
                            }
                            else if (tmpfunexpr.is<string>()){
                                cout << tmpfunexpr.as<string>();
                            }
                            else if (tmpfunexpr.is<Nonetype>()){
                                cout << "None";
                            }
                            else if (tmpfunexpr.is<bool>()){
                                if (tmpfunexpr.as<bool>()) cout << "True";
                                else cout << "False";
                            }
                            if (j != printfunans.size() - 1) cout << ' ';
                        }
                    }
                    if (i != trailerv.size() - 1) cout << ' ';
                }
                cout << endl;
                return 0;
            }
            else if (atomexpr.is<string>()&& atomexpr.as<string>() == "int"){
                antlrcpp::Any tmpatomexpr = trailerv[0];
                if (tmpatomexpr.is<string>()){
                    Bigint tmpintB;
                    tmpintB.createB(tmpatomexpr.as<string>());
                    return tmpintB;
                }
                else if (tmpatomexpr.is<Bigint>()){
                    return tmpatomexpr.as<Bigint>();
                }
                else if (tmpatomexpr.is<double>()){
                    if (tmpatomexpr.as<double>() < 0){
                        return Bigint(int(tmpatomexpr.as<double>()) - 1);
                    }
                    return Bigint(int(tmpatomexpr.as<double>()));
                }
                else if (tmpatomexpr.is<bool>()){
                    return Bigint(int(tmpatomexpr.as<bool>()));
                }
            }
            else if (atomexpr.is<string>()&& atomexpr.as<string>() == "str"){
                antlrcpp::Any tmpatomexpr = trailerv[0];
                if (tmpatomexpr.is<Bigint>()){
                    string B2str;
                    B2str = tmpatomexpr.as<Bigint>().strB();
                    //cout << "in the print" << endl;
                    //cout << B2str;
                    return B2str;
                }
                else if (tmpatomexpr.is<double>()){
                    double d2strdouble = tmpatomexpr.as<double>();
                    string tmpqian = Bigint(int(d2strdouble)).strB();
                    double xiaoshu = d2strdouble - int(d2strdouble);
                    if (xiaoshu == 0){
                        tmpqian += ".000000";
                        return tmpqian;
                    }
                    int cntwei = 0;
                    while(xiaoshu < 1 && cntwei < 6){
                        xiaoshu *= 10;
                        cntwei++;
                    }
                    string d2strans = tmpqian + ".";
                    for (int i = 0; i < cntwei - 1; ++i){
                        d2strans += "0";
                    }
                    for (int i = 0; i < 6 - cntwei; ++i){
                        xiaoshu *= 10;
                    }
                    cout << xiaoshu << endl;
                    string tmphou = Bigint(xiaoshu).strB();
                    d2strans += tmphou;
                    return d2strans;
                }
                else if (tmpatomexpr.is<bool>()){
                    if (tmpatomexpr.as<bool>()){
                        string b2str;
                        b2str = "True";
                        return b2str;
                    } else {
                        string b2str;
                        b2str = "False";
                        return b2str;
                    }
                }
                else if (tmpatomexpr.is<Nonetype>()){
                    string n2str = "None";
                    return n2str;
                }
            }
            else if (atomexpr.is<string>()&& atomexpr.as<string>() == "float"){
                antlrcpp::Any tmpatomexpr = trailerv[0];
                if (tmpatomexpr.is<string>()){
                    double aexprd = 0,cntaexprd = 1;
                    int doubleornot = tmpatomexpr.as<string>().find(".");
                    for (int i = doubleornot - 1; i >= 0; --i){
                        aexprd += (tmpatomexpr.as<string>()[i] - '0') * cntaexprd;
                        cntaexprd *= 10;
                    }
                    cntaexprd = 0.1;
                    for (int i = doubleornot + 1; i < strlen(tmpatomexpr.as<string>().c_str()); ++i){
                        aexprd += (tmpatomexpr.as<string>()[i] - '0') * cntaexprd;
                        cntaexprd /= 10;
                    }
                    return aexprd;
                }
                else if (tmpatomexpr.is<Bigint>()){
                    return double(tmpatomexpr.as<Bigint>());
                }
                else if (tmpatomexpr.is<double>()){
                    return tmpatomexpr.as<double>();
                }
                else if (tmpatomexpr.is<bool>()){
                    return double(tmpatomexpr.as<bool>());
                }
            }
            else if (atomexpr.is<string>()&& atomexpr.as<string>() == "bool"){
                antlrcpp::Any tmpatomexpr = trailerv[0];
                if (tmpatomexpr.is<string>()){
                    if (tmpatomexpr.as<string>() != "") return 1;
                    else return 0;
                }
                else if (tmpatomexpr.is<double>()){
                    return bool(tmpatomexpr.as<double>());
                }
                else if (tmpatomexpr.is<bool>()){
                    return bool(tmpatomexpr.as<bool>());
                }
            }
        } else {
            return visit(ctx->atom());
        }
    }

    antlrcpp::Any visitTrailer(Python3Parser::TrailerContext *ctx) override{
        if (ctx->arglist() != nullptr){
            antlrcpp::Any trailerve =  visit(ctx->arglist());
            return trailerve;
        } else {
            vector<antlrcpp::Any> emptytrailer;
            return emptytrailer;
        }
    }

    antlrcpp::Any visitAtom(Python3Parser::AtomContext *ctx) override{
        //cout << "we have another stupid atom" << endl;
        if (ctx->NAME() != nullptr) {
            string namestr = ctx->getText();
            if (namestr == "print" || namestr == "int" || namestr == "float" || namestr == "bool") return namestr;
            else {
                for (int i = namestack.size() - 1; i >= 0; --i){
                    if (namestack[i].count(namestr) != 0){
                        return namestack[i][namestr];
                    }
                }
                return namestr;
            }
        }
        else if (ctx->NUMBER() != nullptr) {
            string numstr = ctx->getText();
            int doubleornot = numstr.find(".");
            if (doubleornot + 1) {
                double atomd = 0,cntatomd = 1;
                for (int i = doubleornot - 1; i >= 0; --i){
                    atomd += (numstr[i] - '0') * cntatomd;
                    cntatomd *= 10;
                }
                cntatomd = 0.1;
                for (int i = doubleornot + 1; i < strlen(numstr.c_str()); ++i){
                    atomd += (numstr[i] - '0') * cntatomd;
                    cntatomd /= 10;
                }
                return atomd;
            } else {
                Bigint atomB;
                atomB.createB(numstr);
                return atomB;
            }
        }
        else if (ctx->TRUE() != 0) {
            bool atomtrue = 1;
            return atomtrue;
        }
        else if (ctx->FALSE() != 0) {
            bool atomfalse = 0;
            return atomfalse;
        }
        else if (ctx->STRING().size() != 0) {
            string atomstr;
            for (int i = 0; i < ctx->STRING().size(); ++i){
                string terminalstr = ctx->STRING()[i]->getText();
                remove(terminalstr.begin(),terminalstr.end(),'"');
                terminalstr.pop_back();
                terminalstr.pop_back();
                atomstr = atomstr + terminalstr;
            }
            return atomstr;
        }
        else if (ctx->NONE() != nullptr) {
            Nonetype atomnone;
            return atomnone;
        }
        else if (ctx->test() != nullptr) {
            return visit(ctx->test());
        }
    }

    antlrcpp::Any visitTestlist(Python3Parser::TestlistContext *ctx) override{
        vector <antlrcpp::Any> testlistvector;
        for (int i = 0;i < ctx->test().size(); ++i){
            testlistvector.push_back(visit(ctx->test(i)));
        }
        return testlistvector;
    }

    antlrcpp::Any visitArglist(Python3Parser::ArglistContext *ctx) override{
        vector <antlrcpp::Any> arglistlistvector;
        for (int i = 0;i < ctx->argument().size(); ++i){
            arglistlistvector.push_back(visit(ctx->argument(i)));
        }
        return arglistlistvector;
    }

    antlrcpp::Any visitArgument(Python3Parser::ArgumentContext *ctx) override{
        return visit(ctx->test());
    }

    };


#endif //PYTHON_INTERPRETER_EVALVISITOR_H
