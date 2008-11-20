//
//  OperationIntegrate.h
//  mathomatic
//
//  Created by Ben Gotow on 10/25/08.
//  Copyright 2008 __MyCompanyName__. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "Operation.h"


@interface OperationSolve : Operation  <UIActionSheetDelegate>{
}


- (void)run;
- (void)equationsSelected:(NSArray*)activeEquations solveVariables:(NSArray*)activeVariables fromVariables:(NSArray*)allVariables;

@end
