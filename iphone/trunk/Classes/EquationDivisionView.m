//
//  EquationDivisionView.m
//  mathomatic
//
//  Created by Ben Gotow on 11/14/08.
//  Copyright 2008 __MyCompanyName__. All rights reserved.
//

#import "EquationDivisionView.h"
#import "EquationTextView.h"

@implementation EquationDivisionView

- (id)initWithParent:(EquationContainerView*)p {
    if (self = [super initWithParent: p]) {
        innerEquationsMax = 2;
        alignmentMethod = kAlignmentMethodCentered;
    }
    return self;
}

- (void)drawRect:(CGRect)rect {
    // draw the horizontal line across the view, just beneath the first view
    if ([innerEquations count] > 0){
        CGRect          r = [[innerEquations objectAtIndex: 0] frame];
        CGContextRef    c = UIGraphicsGetCurrentContext();
        CGRect          d = CGRectMake(0, r.size.height + 2, [self frame].size.width, 1);
        CGContextFillRect(c, d);
    }
}

- (void)finalizeFrame
{
    // tell our children to finalize their frames so we have accurate width/height measurements
    // for them before continuing.
    [innerEquations makeObjectsPerformSelector:@selector(finalizeFrame)];
    
    // If the division view is complete, then we place the views on top of each other.
    if ([innerEquations count] == 2){
        EquationAbstractView * a = [innerEquations objectAtIndex: 0];
        EquationAbstractView * b = [innerEquations objectAtIndex: 1];
        
        int width = fmax([a frame].size.width, [b frame].size.width);
        
        CGRect aFrame = CGRectMake((width-[a frame].size.width) / 2, 0, [a frame].size.width, [a frame].size.height);
        CGRect bFrame = CGRectMake((width-[b frame].size.width) / 2, aFrame.size.height, [b frame].size.width, [b frame].size.height);
        [a setFrame: aFrame];
        [b setFrame: bFrame];
        
        [self setFrame: CGRectMake(0,0, width, aFrame.size.height + bFrame.size.height)];
    } else {
        // if not, treat this as a regular container
        [super finalizeFrame];
    }
}

- (NSString*)description
{
    if ([innerEquations count] == 2)
        return [NSString stringWithFormat:@"%@/%@", [[self.innerEquations objectAtIndex: 0] description]
                                                  , [[self.innerEquations objectAtIndex: 1] description]];
    else
        return @"/!";
}

- (void)dealloc {
    [super dealloc];
}


@end
