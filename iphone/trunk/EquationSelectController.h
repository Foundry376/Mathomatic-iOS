//
//  EquationSelectController.h
//  mathomatic
//
//  Created by Ben Gotow on 11/13/08.
//  Copyright 2008 __MyCompanyName__. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "OperationSolve.h"

@interface EquationSelectController : UITableViewController {
    NSMutableArray      * equations;
    NSMutableArray      * activeEquations;
    
    NSMutableArray      * possibleVariables;
    NSMutableArray      * visibleVariables;
    NSMutableArray      * activeVariables;
    
    OperationSolve      * delegate;
}

@property (nonatomic, retain) OperationSolve * delegate;

- (id)initWithEquations:(NSArray*)commandHistory andOperation:(OperationSolve*)d;
- (void)reloadVariables;

@end
