// Copyright (c) 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CHROME_BROWSER_UI_COCOA_AUTOFILL_SIMPLE_GRID_LAYOUT_H_
#define CHROME_BROWSER_UI_COCOA_AUTOFILL_SIMPLE_GRID_LAYOUT_H_

#import <Cocoa/Cocoa.h>

#include "base/memory/scoped_ptr.h"
#include "base/memory/scoped_vector.h"

class Column;
class ColumnSet;
class Row;
class ViewState;

// SimpleGridLayout is a layout manager that positions child views in a grid.
// Each row has exactly one ColumnSet, ColumnSets can be shared between rows.
// See ui/views/layout/grid_layout.h for more details - this is a very
// simplified version of the views class.
// This is based on GridLayout in ui/views/layout/grid_layout.h.
// TODO(groby): Unify both classes, create cross-platform abstraction.
// http://crbug.com/240461
class SimpleGridLayout {
 public:
  SimpleGridLayout(NSView* host);
  ~SimpleGridLayout();

  // Creates a new column set with the specified id and returns it.
  // The id is later used when starting a new row.
  // Layout takes ownership of the ColumnSet and will delete it when
  // it is deleted.
  ColumnSet* AddColumnSet(int id);

  // Returns the column set for the specified id, or NULL if one doesn't exist.
  ColumnSet* GetColumnSet(int id);

  // Adds a padding row. Padding rows typically don't have any views, but are
  // used to provide vertical white space between views.
  // |size| specifies the height of the row.
  void AddPaddingRow(int size);

  // Starts a new row with the specified column set.
  void StartRow(float vertical_resize, int column_set_id);

  // This is a convenience function that starts a new row,
  // and returns a new ColumnSet associated with it. All rows created by this
  // will have a height of 0 and resize_percent set to 1.0.
  ColumnSet* AddRow();

  // Advances past columns. Use this when the current column should not
  // contain any views.
  void SkipColumns(int col_count);

  // TODO(groby): This currently *must* be called after a StartRow for the row
  // the view is in. At some point, I'd like an AddView that just populates
  // the next available slot, if possible.
  void AddView(NSView* view);

  // Layout all contained views according to constraints.
  void Layout(NSView* superView);

  void SizeRowsAndColumns(float width);

  // Advances next_column_ past any padding columns.
  void SkipPaddingColumns();

  // Returns the column set of the last non-padding row.
  ColumnSet* GetLastValidColumnSet();

  // Get the height of a given row.
  float GetRowHeight(int row);

  // Y-position for a given row.
  float GetRowLocation(int row_index) const;

  // Get the preferred height for the given width.
  float GetPreferredHeightForWidth(float with);

  int num_rows() const { return static_cast<int>(rows_.size()); }

  // These functions are mostly for testing & deviate from Views Layout class.
  int next_column() { return next_column_; }
  void AdvanceColumn() { next_column_++; }

 private:
  // Adds a new row, updating associated counters and positions.
  void AddRow(Row* row);

  // Next column in the current ColumnSet.
  int next_column_;

  int current_auto_id_;  // Starting value for autogenerated columnset ids.
  ScopedVector<ViewState> view_states_;
  ScopedVector<ColumnSet> column_sets_;
  ScopedVector<Row> rows_;

  NSView* host_;
};

// ColumnSet is used to define a set of columns.
// You don't create a ColumnSet directly, instead use the AddRow method
// of SimpleGridLayout.
class ColumnSet {
 public:
  explicit ColumnSet(int id);
  ~ColumnSet();

  void AddPaddingColumn(int fixed_width);
  void AddColumn(float resize_percent);

  void CalculateSize(float width);
  void ResetColumnXCoordinates();

  // ID of this ColumnSet.
  int id() const { return id_; }

  int num_columns() const { return static_cast<int>(columns_.size()); }

  // Returns the width of the specified columns.
  float GetColumnWidth(int column);

  Column* GetColumn(int column_index) {
    DCHECK(column_index >=0 && column_index < num_columns());
    return columns_[column_index];
  }

  // These functions are mostly for testing & deviate from Views Layout class.
  float ColumnLocation(int column_index);

 private:
  float CalculateRemainingWidth(float width);
  void DistributeRemainingWidth(float width);

  ScopedVector<Column> columns_;
  int id_;
};

#endif  // CHROME_BROWSER_UI_COCOA_AUTOFILL_SIMPLE_GRID_LAYOUT_H_