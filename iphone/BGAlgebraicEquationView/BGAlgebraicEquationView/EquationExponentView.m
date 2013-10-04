//
//  EquationExponentView.m
//  mathomatic
//
//  Created by Ben Gotow on 11/14/08.
//  Copyright 2008 __MyCompanyName__. All rights reserved.
//

#import "EquationExponentView.h"
#import "EquationTextView.h"

@implementation EquationExponentView

- (id)initWithParent:(EquationContainerView*)p {
    if (self = [super initWithParent: p]) {
        innerEquationsMax = 2;
        
        [self setBackgroundColor: [UIColor clearColor]];
    }
    return self;
}

- (void)finalizeTextSize:(int)parentTextSize
{
    if ([innerEquations count] == 2){
        [[innerEquations objectAtIndex: 0] finalizeTextSize: parentTextSize];
        [[innerEquations objectAtIndex: 1] finalizeTextSize: parentTextSize * 0.75];
    } else 
        [super finalizeTextSize: parentTextSize];
}

- (void)finalizeFrame
{
    // tell our children to finalize their frames so we have accurate width/height measurements
    // for them before continuing.
    [innerEquations makeObjectsPerformSelector:@selector(finalizeFrame)];
 
    // if the container is full, we want to organize the subviews to put the B view in the exponent location.
    if ([innerEquations count] == 2){
        EquationAbstractView * a = [innerEquations objectAtIndex: 0];
        EquationAbstractView * b = [innerEquations objectAtIndex: 1];
        
        CGRect aFrame = CGRectMake(0, [b frame].size.height * 0.5, [a frame].size.width, [a frame].size.height);
        CGRect bFrame = CGRectMake(aFrame.size.width, 0, [b frame].size.width, [b frame].size.height);
        [a setFrame: aFrame];
        [b setFrame: bFrame];
        
        [self setFrame: CGRectUnion(aFrame, bFrame)];
    } else
        [super finalizeFrame];
}

- (NSString*)description
{
    if ([self.innerEquations count] == 2)
        return [NSString stringWithFormat:@"%@^%@", [[self.innerEquations objectAtIndex: 0] description]
                                                    , [[self.innerEquations objectAtIndex: 1] description]];
    else 
        return @"^!";
}

- (void)dealloc {
    [super dealloc];
}


@end
