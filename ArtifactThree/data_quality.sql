-- ============================================================
-- 02_data_quality.sql
-- Data Quality Enhancements
-- Author: Jason Carignan
-- Purpose:
--   1) Create ReturnReason reference table
--   2) Add ReasonCode to RMA
--   3) Map free-text reasons -> standardized codes
-- ============================================================

-- ------------------------------------------------------------
-- Switch to updated DB structure created in schema.sql
-- ------------------------------------------------------------
USE QuantigrationRMA_Enhanced;

-- ------------------------------------------------------------
-- Return reason reference
-- ------------------------------------------------------------
CREATE TABLE IF NOT EXISTS ReturnReason (
  ReasonCode        VARCHAR(20)  NOT NULL PRIMARY KEY,
  ReasonDescription VARCHAR(100) NOT NULL
);

-- Seed the reference table
INSERT IGNORE INTO ReturnReason (ReasonCode, ReasonDescription) VALUES
('INCORRECT', 'Incorrect item / wrong product'),
('DEFECTIVE', 'Defective item'),
('OTHER',     'Other reason');

-- ------------------------------------------------------------
-- Add ReasonCode column to RMA (if not already present)
-- Since MySQL doesn't support ADD COLUMN IF NOT EXISTS so I do a safe check.
-- ------------------------------------------------------------
SET @col_exists :=
(
  SELECT COUNT(*)
  FROM information_schema.columns
  WHERE table_schema = DATABASE()
    AND table_name = 'RMA'
    AND column_name = 'ReasonCode'
);

SET @sql := IF(@col_exists = 0,
  'ALTER TABLE RMA ADD COLUMN ReasonCode VARCHAR(20) NULL;',
  'SELECT ''ReasonCode already exists'';'
);

PREPARE stmt FROM @sql;
EXECUTE stmt;
DEALLOCATE PREPARE stmt;

-- ------------------------------------------------------------
-- Map free-text Reason -> standardized ReasonCode
-- (Keeps original Reason column intact for traceability)
-- ------------------------------------------------------------
UPDATE RMA
SET ReasonCode =
  CASE
    WHEN Reason = 'Incorrect' THEN 'INCORRECT'
    WHEN Reason = 'Defective' THEN 'DEFECTIVE'
    WHEN Reason = 'Other'     THEN 'OTHER'
    ELSE 'OTHER'
  END;

-- ------------------------------------------------------------
-- Validation checks
-- ------------------------------------------------------------
SELECT Reason, ReasonCode, COUNT(*) AS cnt
FROM RMA
GROUP BY Reason, ReasonCode
ORDER BY cnt DESC;

SELECT ReasonCode, COUNT(*) AS cnt
FROM RMA
GROUP BY ReasonCode
ORDER BY cnt DESC;
