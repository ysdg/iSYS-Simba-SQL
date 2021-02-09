#pragma once

#include "IsysCommon.h"
#include "DSIExtOperationHandlerFactory.h"
#include "DSIExtResultSet.h"
#include "Quickstart.h"

ISYS_SQL_NAMESPACE_BEGIN

class COperationHandlerFactory : public Simba::SQLEngine::DSIExtOperationHandlerFactory
{
public:
    /// @brief Constructor.
    COperationHandlerFactory(Simba::Quickstart::QuickstartSettings* in_Settings);

    /// @brief Destructor.
    virtual ~COperationHandlerFactory() {}

    /// @brief Create an AGGREGATION handler to handle passing down an AGGREGATION operation on
    /// the given table.
    /// 
    /// @param in_table         The table on which the aggregation operates. Cannot be NULL.
    ///
    /// @return an AGGREGATION handler to handle passing down AGGREGATION operations if the
    ///         feature is supported, NULL otherwise. (OWN)
    virtual AutoPtr<Simba::SQLEngine::IAggregationHandler> CreateAggregationHandler(
        SharedPtr<Simba::SQLEngine::DSIExtResultSet> in_table);

    /// @brief Create a filter handler to handle passing down filters onto the table.
    ///
    /// @param in_table         The table on which to apply filters. Cannot be NULL.
    ///
    /// @return a filter handler to handle passing down filters if the feature is supported, 
    ///         NULL otherwise. (OWN)
    virtual AutoPtr<Simba::SQLEngine::IBooleanExprHandler> CreateFilterHandler(
        SharedPtr<Simba::SQLEngine::DSIExtResultSet> in_table);

    /// @brief Create a JOIN condition handler to handle passing down JOIN operations.
    ///
    /// @param in_tableLeft     The left table of the join. Cannot be NULL.
    /// @param in_tableRight    The right table of the join. Cannot be NULL.
    /// @param in_joinType      The type of the join being applied.
    ///
    /// @return a JOIN handler condition to handle passing down JOIN operations if the feature 
    ///         is supported, NULL otherwise. (OWN)
    //virtual AutoPtr<Simba::SQLEngine::IBooleanExprHandler> CreateJoinHandler(
    //    SharedPtr<Simba::SQLEngine::DSIExtResultSet> in_tableLeft,
    //    SharedPtr<Simba::SQLEngine::DSIExtResultSet> in_tableRight,
    //    Simba::SQLEngine::AEJoinType in_joinType);

    // Private =====================================================================================
private:
    /// Struct containing CodeBase settings and error codes. (NOT OWN)
    Simba::Quickstart::QuickstartSettings* m_settings;
};

ISYS_SQL_NAMESPACE_END
