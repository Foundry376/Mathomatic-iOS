//
//  MathomaticExpression.h
//  mathomatic
//
//  Created by Ben Gotow on 11/20/08.
//  Copyright 2008 __MyCompanyName__. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "EquationView.h"

@interface MathomaticExpression : NSObject {

    NSString        * mathomaticText;
    NSString        * equationText;
    EquationView    * equation;
}

@synthesize (nonatomic, retain) NSString * mathomaticText;
@synthesize (nonatomic, retain) NSString * equationText;
@synthesize (nonatomic, retain) EquationView * equation;

@end
