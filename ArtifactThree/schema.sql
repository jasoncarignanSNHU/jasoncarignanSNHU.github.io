-- ============================================================
-- schema.sql
-- QuantigrationRMA Enhanced Database Setup
-- Author: Jason Carignan
-- Purpose:
--   1) Create enhanced database
--   2) Create base tables (Customers, Orders, RMA, Collaborators)
--   3) Copy data from QuantigrationRMA into QuantigrationRMA_Enhanced
-- ============================================================

CREATE DATABASE IF NOT EXISTS QuantigrationRMA_Enhanced;
USE QuantigrationRMA_Enhanced;

-- ------------------------------------------------------------
-- Drop objects:
-- Used to recreate if necessary for cleanup
-- ------------------------------------------------------------
-- DROP TABLE IF EXISTS RMA;
-- DROP TABLE IF EXISTS Orders;
-- DROP TABLE IF EXISTS Customers;
-- DROP TABLE IF EXISTS Collaborators;

-- ------------------------------------------------------------
-- Create Customers / Orders / RMA by cloning structure
-- ------------------------------------------------------------
CREATE TABLE IF NOT EXISTS Customers LIKE QuantigrationRMA.Customers;
CREATE TABLE IF NOT EXISTS Orders    LIKE QuantigrationRMA.Orders;
CREATE TABLE IF NOT EXISTS RMA       LIKE QuantigrationRMA.RMA;

-- ------------------------------------------------------------
-- Create Collaborators as a real base table (manual schema)
-- Because QuantigrationRMA.Collaborators is not a base table.
-- Structure mirrors DESCRIBE Collaborators output.
-- ------------------------------------------------------------
CREATE TABLE IF NOT EXISTS Collaborators (
  CollaboratorID INT NOT NULL,
  FirstName      VARCHAR(25),
  Telephone      VARCHAR(15),
  LastName       VARCHAR(25),
  Street         VARCHAR(50),
  ZipCode        VARCHAR(10),
  City           VARCHAR(50),
  State          VARCHAR(25)
);

-- ------------------------------------------------------------
-- Copy data, using INSERT IGNORE to avoid duplication if rerun
-- ------------------------------------------------------------
INSERT IGNORE INTO Customers SELECT * FROM QuantigrationRMA.Customers;
INSERT IGNORE INTO Orders    SELECT * FROM QuantigrationRMA.Orders;
INSERT IGNORE INTO RMA       SELECT * FROM QuantigrationRMA.RMA;

-- Collaborators was a view in the original DB, but we duplicated it here with a simple SELECT.
INSERT IGNORE INTO Collaborators SELECT * FROM QuantigrationRMA.Collaborators;

-- ------------------------------------------------------------
-- Sanity row counts
-- ------------------------------------------------------------
SELECT 'Customers' AS table_name, COUNT(*) AS row_count FROM Customers
UNION ALL
SELECT 'Orders', COUNT(*) FROM Orders
UNION ALL
SELECT 'RMA', COUNT(*) FROM RMA
UNION ALL
SELECT 'Collaborators', COUNT(*) FROM Collaborators;
