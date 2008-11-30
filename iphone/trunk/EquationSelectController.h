//
//  EquationSelectController.h
//  mathomatic
//
//  Created by Ben Gotow on 11/13/08.
//  Copyright 2008 __MyCompanyName__. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "MathomaticOperation.h"

@protocol EquationSelectDelegate
- (void)equationsSelected:(NSArray*)equations solveVariable:(NSString*)var;
@end

@interface EquationSelectController : UITableViewController {
    
    NSMutableArray              * equations;
    NSMutableArray              * equationCells;
    NSMutableArray              * activeEquations;
    
    NSMutableArray              * variables;
    int                           activeVariable;
    
    id<EquationSelectDelegate>    delegate;
    
    NSMutableArray              * cells;
}

@property (nonatomic, retain) NSArray * equations;
@property (nonatomic, retain) id<EquationSelectDelegate> delegate;

- (id)initWithEquations:(NSArray*)commandHistory andOperation:(id<EquationSelectDelegate>)d;
- (void)reloadVariables;

@end
