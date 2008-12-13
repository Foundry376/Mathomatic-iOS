//
//  MathomaticOperationSolve.h
//  mathomatic
//
//  Created by Ben Gotow on 11/21/08.
//  Copyright 2008 __MyCompanyName__. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "MathomaticOperation.h"
#import "EquationSelectController.h"

@interface MathomaticOperationSolve : MathomaticOperation <EquationSelectDelegate> {

    NSString * variable;
}

- (id)init;
- (NSString*)perform;
- (NSString*)prepareWithDelegate:(MathomaticViewController*)c;
- (void)equationsSelected:(NSArray*)equations solveVariable:(NSString*)var;
- (void)equationSelectCancelled;

@end
