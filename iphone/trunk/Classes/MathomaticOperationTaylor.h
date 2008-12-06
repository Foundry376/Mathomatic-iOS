//
//  MathomaticOperationSolve.h
//  mathomatic
//
//  Created by Ben Gotow on 11/21/08.
//  Copyright 2008 __MyCompanyName__. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "MathomaticOperation.h"

@interface MathomaticOperationTaylor : MathomaticOperation <UIActionSheetDelegate>  {

    NSString * variable;
    float      point;
    int        order;
}

- (void)parametersSelected:(NSString*)var point:(float)p order:(int)o;

@end
